#include <ApplicationLayer.h>

#include "LightView.h"
#include "GameProcess.h"
#include "ParticleComponent.h"
#include "ComponentIDs.h"
#include "ButtonComponent.h"
#include "ComponentIDs.h"
#include "TransformComponent.h"
#include "UIImage.h"

using namespace Bel;

bool LightView::Initialize()
{
    m_pTransform = static_cast<TransformComponent*>(GetActor()->GetComponent(kTransformId));
    if (!m_pTransform)
    {
        LOG_FATAL("Unabled to acquire TransformComponent from Actor Id:", false);
        LOG_FATAL(std::to_string(GetActor()->GetId()));
        return false;
    }

    CreateMoveProcess();
    m_processManager.AttachProcess(m_pMoveProcess);

    return true;
}

void LightView::UpdateInput(float delta)
{
    m_processManager.UpdateProcesses(delta);
}

void LightView::CreateDelayProcess()
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
            LOG_FATAL("A Process has been aborted: Delay from LightView");
        });
}

void LightView::CreateMoveProcess()
{
    m_pMoveProcess = std::make_shared<MoveProcess>(
        GetActor(),
        m_pTransform->GetX(),
        m_positions[m_currentPosition],
        5.0f);
    m_pMoveProcess->SetSuccessCallback(
        [this]()
        {
            CreateDelayProcess();
            m_pMoveProcess->AttachChild(m_pDelayProcess);
        });

    m_currentPosition = (m_currentPosition + 1) % kNumPositions;
}
