#include <ApplicationLayer.h>

#include "NinjaView.h"
#include "GameProcess.h"
#include "ParticleComponent.h"
#include "ComponentIDs.h"
#include "ButtonComponent.h"
#include "ComponentIDs.h"
#include "UIImage.h"

using namespace Bel;

bool NinjaView::Initialize()
{
    m_pMouse = ApplicationLayer::GetInstance()->GetMouseInput();

    m_pParticle = static_cast<ParticleComponent*>(GetActor()->GetComponent(kParticleId));
    if (!m_pParticle)
    {
        LOG_ERROR("Failed to acquire ParticleComponent from actor id: ", false);
        LOG_ERROR(std::to_string(GetActor()->GetId()));
        return false;
    }

    // --- Buttons ---
    {
        auto pGameLayer = ApplicationLayer::GetInstance()->GetGameLayer();
        auto pActorFactory = pGameLayer->GetActorFactory();

        // --- StartDance Button
        {
            auto pStartDanceBtn = pGameLayer->GetActorFactory().CreateActor("GUI/Button.xml");
            if (pStartDanceBtn)
            {
                m_pStartDanceBtn = static_cast<ButtonComponent*>(pStartDanceBtn->GetComponent(kButtonId));
                if (!m_pStartDanceBtn)
                {
                    LOG_FATAL("Unabled to acquire ButtonComponent from Actor Id:", false);
                    LOG_FATAL(std::to_string(m_pStartDanceBtn->GetId()));
                    return false;
                }

                auto pUIimage = static_cast<UIImage*>(pStartDanceBtn->GetComponent(kUIImageId));
                if (!pUIimage)
                {
                    LOG_FATAL("Unabled to acquire UIImage from Actor Id:", false);
                    LOG_FATAL(std::to_string(m_pStartDanceBtn->GetId()));
                    return false;
                }
                pUIimage->SetColor(255, 0, 0);

                m_pStartDanceBtn->SetClickEnter
                (
                    [this]()
                    {
                        m_processManager.AbortAllProcesses();
                        CreateDelayProcess();
                        m_processManager.AttachProcess(m_pDelayProcess);
                    }
                );

                auto pBtnTransform = static_cast<Bel::TransformComponent*>(pStartDanceBtn->GetComponent(kTransformId));
                if (!pBtnTransform)
                {
                    LOG_FATAL("Unabled to acquire ButtonComponent from Actor Id:", false);
                    LOG_FATAL(std::to_string(pStartDanceBtn->GetId()));
                    return false;
                }
                pBtnTransform->SetPosition(600, 400);
                
                pGameLayer->AddGUI(pStartDanceBtn->GetId(), pStartDanceBtn);
            }
        }

        // --- StopDance Button
        {
            auto pStopDanceBtn = pGameLayer->GetActorFactory().CreateActor("GUI/Button.xml");
            if (pStopDanceBtn)
            {
                m_pStopDanceBtn = static_cast<ButtonComponent*>(pStopDanceBtn->GetComponent(kButtonId));
                if (!m_pStopDanceBtn)
                {
                    LOG_FATAL("Unabled to acquire ButtonComponent from Actor Id:", false);
                    LOG_FATAL(std::to_string(pStopDanceBtn->GetId()));
                    return false;
                }

                auto pUIimage = static_cast<UIImage*>(pStopDanceBtn->GetComponent(kUIImageId));
                if (!pUIimage)
                {
                    LOG_FATAL("Unabled to acquire UIImage from Actor Id:", false);
                    LOG_FATAL(std::to_string(m_pStartDanceBtn->GetId()));
                    return false;
                }
                pUIimage->SetColor(0, 255, 0);

                m_pStopDanceBtn->SetClickEnter
                (
                    [this]()
                    {
                        m_processManager.AbortAllProcesses();

                        CreatePatrolProcess();
                        m_processManager.AttachProcess(m_pPatrolProcess);
                    }
                );

                auto pBtnTransform = static_cast<Bel::TransformComponent*>(pStopDanceBtn->GetComponent(kTransformId));
                if (!pBtnTransform)
                {
                    LOG_FATAL("Unabled to acquire ButtonComponent from Actor Id:", false);
                    LOG_FATAL(std::to_string(pStopDanceBtn->GetId()));
                    return false;
                }
                pBtnTransform->SetPosition(600, 500);
            }
            pGameLayer->AddGUI(pStopDanceBtn->GetId(), pStopDanceBtn);
        }

        // --- Confetti Button
        {
            auto pConfettiBtn = pGameLayer->GetActorFactory().CreateActor("GUI/Button.xml");
            if (pConfettiBtn)
            {
                m_pConfettiBtn = static_cast<ButtonComponent*>(pConfettiBtn->GetComponent(kButtonId));
                if (!m_pConfettiBtn)
                {
                    LOG_FATAL("Unabled to acquire ButtonComponent from Actor Id:", false);
                    LOG_FATAL(std::to_string(pConfettiBtn->GetId()));
                    return false;
                }

                auto pUIimage = static_cast<UIImage*>(pConfettiBtn->GetComponent(kUIImageId));
                if (!pUIimage)
                {
                    LOG_FATAL("Unabled to acquire UIImage from Actor Id:", false);
                    LOG_FATAL(std::to_string(m_pStartDanceBtn->GetId()));
                    return false;
                }
                pUIimage->SetColor(0, 0, 255);

                m_pConfettiBtn->SetClickEnter
                (
                    [this]()
                    {
                        m_processManager.AbortAllProcesses();
                        auto pConfettiProcess = std::make_shared<SpreyConfettiProcess>(m_pParticle, 300);
                        pConfettiProcess->SetAbortCallback
                        (
                            [this]()
                            {
                                LOG_FATAL("A Process has been aborted: SpreyConfettiProcess from PlayerView");
                            }
                        );

                        m_processManager.AttachProcess(pConfettiProcess);
                    }
                );

                auto pBtnTransform = static_cast<Bel::TransformComponent*>(pConfettiBtn->GetComponent(kTransformId));
                if (!pBtnTransform)
                {
                    LOG_FATAL("Unabled to acquire ButtonComponent from Actor Id:", false);
                    LOG_FATAL(std::to_string(pConfettiBtn->GetId()));
                    return false;
                }
                pBtnTransform->SetPosition(600, 600);
                pGameLayer->AddGUI(pConfettiBtn->GetId(), pConfettiBtn);
            }
        }
    }


    return true;
}

void NinjaView::UpdateInput(float delta)
{
    m_processManager.UpdateProcesses(delta);
}

void NinjaView::CreateDelayProcess()
{
    m_pDelayProcess = std::make_shared<DelayProcess>(3.0f); // 3 seconds
    m_pDelayProcess->SetSuccessCallback(
        [this]()
        {
            CreateMoveProcess();
            m_pDelayProcess->AttachChild(m_pMoveProcess);
        });

    m_pDelayProcess->SetAbortCallback(
        [this]()
        {
            LOG_FATAL("A Process has been aborted: Delay from NinjaView");
        });
}

void NinjaView::CreateConfettiProcess()
{
    m_pConfettiProcess = std::make_shared<SpreyConfettiProcess>(m_pParticle, 300);
    m_pConfettiProcess->SetSuccessCallback(
        [this]()
        {
            CreatePatrolProcess();
            m_pConfettiProcess->AttachChild(m_pPatrolProcess);
        });
    m_pConfettiProcess->SetFailureCallback(
        [this]()
        {
            CreatePatrolProcess();
            m_pConfettiProcess->AttachChild(m_pPatrolProcess);
            LOG_FATAL("A Process has been failed: ConfettiProcess from NinjaView");
        });
}

void NinjaView::CreateMoveProcess()
{
    m_pMoveProcess = std::make_shared<MoveWithAnimProcess>(
        GetActor(),
        m_positions[m_currentPosition][0],
        m_positions[m_currentPosition][1],
        3.0f);
    m_pMoveProcess->SetSuccessCallback(
        [this]()
        {
            CreateDelayProcess();
            m_pMoveProcess->AttachChild(m_pDelayProcess);
        });

    m_currentPosition = (m_currentPosition + 1) % kNumPositions;
}

void NinjaView::CreatePatrolProcess()
{
    m_pPatrolProcess = std::make_shared<PatrolProcess>(GetActor(), "Processes/DancerPatrol.xml");
    m_pPatrolProcess->SetSuccessCallback(
        [&]()
        {
            CreateConfettiProcess();
            m_pPatrolProcess->AttachChild(m_pConfettiProcess);
        });
    m_pPatrolProcess->SetAbortCallback(
        [&]()
        {
            LOG_FATAL("ConfettiProcess: Aborted");
        });
}

void NinjaView::CreateJumpProcess()
{
    m_pJumpProcess = std::make_shared<JumpProcess>(GetActor(), 20.0f, 4.0f);
    m_pJumpProcess->SetSuccessCallback(
        [&]()
        {
            CreateConfettiProcess();
            m_pJumpProcess->AttachChild(m_pConfettiProcess);
        });
}
