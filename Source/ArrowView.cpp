#include <ApplicationLayer.h>

#include "GameProcess.h"
#include "ArrowView.h"
#include "GameEvents.h"
#include "ComponentIDs.h"
#include "TransformComponent.h"
#include "HealthComponent.h"

using namespace Bel;

bool ArrowView::Initialize()
{
    m_pTransform = static_cast<TransformComponent*>(GetActor()->GetComponent(kTransformId));
    if (m_pTransform == nullptr)
    {
        LOG_FATAL("Failed to acquire TransformComponent for Bullet. ID: ", false);
        LOG_FATAL(std::to_string(GetActor()->GetId()).c_str());
        return false;
    }


    auto pBulletMove = std::make_shared<ConstantMovingProcess>(GetActor(), 9, m_headToRight);
    m_processManager.AttachProcess(pBulletMove);


    m_pLifeTime = std::make_shared<DelayProcess>(4.0f);
    m_pLifeTime->SetSuccessCallback
    (
        [this]()
        {
            ApplicationLayer::GetInstance()->GetGameLayer()->GetEventManager().QueueEvent(std::make_unique<DestroyActorEvent>(GetActor()->GetId()));
            ApplicationLayer::GetInstance()->GetGameLayer()->GetEventManager().QueueEvent(std::make_unique<DestroyViewEvent>(this));
        }
    );
    m_processManager.AttachProcess(m_pLifeTime);

    ApplicationLayer::GetInstance()->GetGameLayer()->GetEventManager().AddEventListener(ReachTheGoalEvent::kEventType, std::bind(&ArrowView::RemoveAll, this, std::placeholders::_1));


    return true;
}

void ArrowView::UpdateInput(float delta)
{
    m_processManager.UpdateProcesses(delta);
}

void ArrowView::OnCollisionEnter(Bel::Actor* pOther, bool isSensor)
{
    if (pOther->IsName("Mario"))
    {
        HealthComponent* pMarioHealth = static_cast<HealthComponent*>(pOther->GetComponent(kHealthId));
        if (pMarioHealth == nullptr)
        {
            LOG_FATAL("Failed to acquire HealthComponent from Mario. ID: ", false);
            LOG_FATAL(std::to_string(pOther->GetId()).c_str());
            return;
        }

        pMarioHealth->TakeDamage(1);

        if (pMarioHealth->GetPoint() < 0)
        {
            pMarioHealth->GainPoint(3);
            ApplicationLayer::GetInstance()->GetGameLayer()->GetEventManager().QueueEvent(std::make_unique<TeleportActorEvent>(-150, 0));
        }

        m_canMove = false;

        ApplicationLayer::GetInstance()->GetGameLayer()->GetEventManager().QueueEvent(std::make_unique<DestroyActorEvent>(GetActor()->GetId()));
        ApplicationLayer::GetInstance()->GetGameLayer()->GetEventManager().QueueEvent(std::make_unique<DestroyViewEvent>(this));
        //m_processManager.AbortAllProcesses();
    }
}

void ArrowView::OnCollisionExit(Bel::Actor* pOther, bool isSensor)
{
}

void ArrowView::RemoveAll(Bel::IEvent* pEvent)
{
    m_processManager.AbortAllProcesses();
    ApplicationLayer::GetInstance()->GetGameLayer()->GetEventManager().QueueEvent(std::make_unique<DestroyViewEvent>(this));

}
