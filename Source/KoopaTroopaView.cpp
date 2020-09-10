#include <Core/Layers/ApplicationLayer.h>
#include <math.h>

#include "KoopaTroopaView.h"
#include "ComponentIDs.h"
#include "AnimationComponent.h"
#include "TextureComponent.h"
#include "GameEvents.h"
#include "GameProcess.h"
#include "HealthComponent.h"
#include "Attackable.h"
#include "TagComponent.h"
#include "MarioStates.h"
#include "AudioSource.h"

using namespace Bel;

KoopaTroopaView::KoopaTroopaView()
    : m_canAttack(true)
{
}

bool KoopaTroopaView::Initialize()
{
    // --- Actor Components ---
    {
        auto& logging = ApplicationLayer::GetInstance()->GetLogging();
        logging.AddCategory(Logging::SeverityLevel::kLevelFatal, "KoopaTroopaView", true);

        auto pActor = GetActor();
        if (!pActor->HasComponent(kTransformId))
        {
            logging.Log(Logging::SeverityLevel::kLevelFatal, "Unable to finc TransformComponent from Koopa Troopa", true, "KoopaTroopaView");
            return false;
        }

        m_pTransform = static_cast<TransformComponent*>(pActor->GetComponent(kTransformId));

        if (!pActor->HasComponent(kAnimationId))
        {
            logging.Log(Logging::SeverityLevel::kLevelFatal, "Unable to finc AnimationComponent from Koopa Troopa", true, "KoopaTroopaView");
            return false;
        }

        m_pAnim = static_cast<AnimationComponent*>(pActor->GetComponent(kAnimationId));
        m_pAnim->SetAnimation("Walk");

        if (!pActor->HasComponent(kTextureId))
        {
            logging.Log(Logging::SeverityLevel::kLevelFatal, "Unable to finc TextureComponent from Koopa Troopa", true, "KoopaTroopaView");
            return false;
        }

        m_pTexture = static_cast<TextureComponent*>(pActor->GetComponent(kTextureId));
        m_pTransform->SetPosition(static_cast<float>((rand() % 1800) + 300), static_cast<float>((rand() % 500)));
    }

    auto pGameLayer = ApplicationLayer::GetInstance()->GetGameLayer();
    pGameLayer->GetEventManager().AddEventListener(ChaseTargetEvent::kEventType, std::bind(&KoopaTroopaView::ChaseTarget, this, std::placeholders::_1));
    pGameLayer->GetEventManager().AddEventListener(ReachGoalEvent::kEventType, std::bind(&KoopaTroopaView::ReachGoal, this, std::placeholders::_1));
    return true;
}

void KoopaTroopaView::UpdateInput(float delta)
{
    m_processManager.UpdateProcesses(delta);
}

void KoopaTroopaView::ViewScene()
{
}

void KoopaTroopaView::Delete()
{
    auto pGameLayer = Bel::ApplicationLayer::GetInstance()->GetGameLayer();
    for (auto [guid, id] : m_listenerIds)
    {
        pGameLayer->GetEventManager().RemoveEventListener(guid, id);
    }

    m_processManager.AbortAllProcesses();
}

void KoopaTroopaView::OnCollisionEnter(Collision& col)
{
    Actor* pOther = col.m_pActor;

    if (pOther->IsName("Mario"))
    {
        auto pMarioState = static_cast<MarioStateMachine*>(pOther->GetComponent(kMarioState));
        if (pMarioState == nullptr)
        {
            LOG_FATAL("Failed to get StateMachine from Mario : OnCollisionEnter() in KoopaTroopaView");
            return;
        }

        auto pAudioSource = static_cast<AudioSource*>(pOther->GetComponent(kAudioSourceId));
        if (pAudioSource == nullptr)
        {
            LOG_FATAL("Failed to get AudioSource from Mario : OnCollisionEnter() in KoopaTroopaView");
            return;
        }

        if (m_canAttack)
        {
            m_canAttack = false;
            const std::string& stateName = pMarioState->GetCurrentStateName();
            if (stateName.find("Super") != std::string::npos)
            {
                pMarioState->ChangeState("SmallState");
                pMarioState->HandleInput();
                pAudioSource->PlayClip("PipeAndPowerDown");
            }
            else
            {
                ApplicationLayer::GetInstance()->GetGameLayer()->GetEventManager().QueueEvent(std::make_unique<TeleportActorEvent>(pOther->GetId(), 0, 250));
            }

            auto pCooldown = std::make_shared<DelayProcess>(1.5f);
            pCooldown->SetSuccessCallback
            (
                [this]()
                {
                    m_canAttack = true;
                }
            );
            m_processManager.AttachProcess(pCooldown);
        }
    }

    TagComponent* pTag = static_cast<TagComponent*>(pOther->GetComponent(kTagId));
    if (pTag)
    {
        const std::string_view tagName = pTag->GetTag();
        if (tagName == "Projectile")
        {
            if (pOther->IsName("BowserFire"))
                return;

            DeleteActorAndView();
            ApplicationLayer::GetInstance()->GetGameLayer()->GetEventManager().QueueEvent(std::make_unique<DestroyActorEvent>(pOther->GetId()));
        }
    }
}

void KoopaTroopaView::OnCollisionExit(Collision& col)
{
}

void KoopaTroopaView::OnTriggerEnter(Collision& col)
{
    Actor* pOther = col.m_pActor;

    if (pOther->IsName("Mario"))
    {
        ApplicationLayer::GetInstance()->GetGameLayer()->GetEventManager().QueueEvent(std::make_unique<ChaseTargetEvent>(GetActor()->GetId(), pOther->GetId()));
    }
}

void KoopaTroopaView::ChaseTarget(Bel::IEvent* pEvent)
{
    auto pChaseTarget = static_cast<ChaseTargetEvent*>(pEvent);

    Actor::Id actorId = pChaseTarget->GetManipualtedId();
    auto pActor = ApplicationLayer::GetInstance()->GetGameLayer()->GetActor(pChaseTarget->GetManipualtedId());
    if (!pActor || GetActor()->GetId() != actorId)
    {
        m_processManager.AbortAllProcesses();
        return;
    }

    Actor::Id targetId = pChaseTarget->GetTargetId();
    auto actors = ApplicationLayer::GetInstance()->GetGameLayer()->GetActors();
    auto pTargetActor = actors[targetId];
    
    if (pTargetActor == nullptr)
        return;

    TransformComponent* pTargetTransform = static_cast<TransformComponent*>(pTargetActor->GetComponent(kTransformId));

    auto pChase = std::make_shared<ChasingProcess>(m_pTransform, pTargetTransform, m_pTexture);
    m_processManager.AttachProcess(pChase);
}

void KoopaTroopaView::DeleteActorAndView()
{
    ApplicationLayer::GetInstance()->GetGameLayer()->GetEventManager().QueueEvent(std::make_unique<DestroyActorEvent>(GetActor()->GetId()));
    ApplicationLayer::GetInstance()->GetGameLayer()->GetEventManager().QueueEvent(std::make_unique<RemoveViewEvent>(this));
}

void KoopaTroopaView::ReachGoal(Bel::IEvent* pEvent)
{
    Delete();
    ApplicationLayer::GetInstance()->GetGameLayer()->GetEventManager().QueueEvent(std::make_unique<RemoveViewEvent>(this));
}
