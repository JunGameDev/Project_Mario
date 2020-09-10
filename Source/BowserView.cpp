#include <Core/Layers/ApplicationLayer.h>

#include "BowserView.h"
#include "TextureComponent.h"
#include "AnimationComponent.h"
#include "AudioSource.h"
#include "ComponentIDs.h"
#include "ProjectileView.h"
#include "TagComponent.h"

#include "GameEvents.h"
#include "GameProcess.h"

using namespace Bel;

bool BowserView::Initialize()
{
    m_pGameLayer = ApplicationLayer::GetInstance()->GetGameLayer();
    if (!m_pGameLayer)
    {
        LOG_FATAL("Failed to acquire GameLayer.");
        return false;
    }

    auto pActor = GetActor();
    m_pActorTransform = static_cast<Bel::TransformComponent* > (pActor->GetComponent(kTransformId));
    if (!m_pActorTransform)
    {
        LOG_FATAL("Failed to find TransformComponent for actor id: ", false);
        LOG_FATAL(std::to_string(pActor->GetId()).c_str());
        return false;
    }

    m_pAudioSource = static_cast<AudioSource*>(pActor->GetComponent(kAudioSourceId));
    if (!m_pAudioSource)
    {
        LOG_FATAL("Failed to find AudioSource for actor id: ", false);
        LOG_FATAL(std::to_string(pActor->GetId()).c_str());
        return false;
    }

    m_pTexture = static_cast<TextureComponent*>(pActor->GetComponent(kTextureId));
    if (!m_pTexture)
    {
        LOG_FATAL("Failed to find TextureComponent for actor id: ", false);
        LOG_FATAL(std::to_string(pActor->GetId()).c_str());
        return false;
    }

    m_pActorAnim = static_cast<AnimationComponent*>(pActor->GetComponent(kAnimationId));
    if (!m_pActorAnim)
    {
        LOG_FATAL("Failed to find AnimationCompontn for actor id: ", false);
        LOG_FATAL(std::to_string(pActor->GetId()).c_str());
        return false;
    }

    m_pBody = static_cast<IDynamicBodyComponent*>(pActor->GetComponent(kDynamicBodyId));
    if (!m_pBody)
    {
        LOG_FATAL("Failed to find IDynamicBodyComponent for actor id: ", false);
        LOG_FATAL(std::to_string(pActor->GetId()).c_str());
        return false;
    }

    m_listenerIds[CreateActorAtLocationEvent::kEventType]
        = m_pGameLayer->GetEventManager().AddEventListener(CreateActorAtLocationEvent::kEventType, std::bind(&BowserView::SpawnProjectile, this, std::placeholders::_1));
    m_listenerIds[ReachGoalEvent::kEventType]
        = m_pGameLayer->GetEventManager().AddEventListener(ReachGoalEvent::kEventType, std::bind(&BowserView::ReachGoal, this, std::placeholders::_1));

    m_pActorTransform->SetPosition(static_cast<float>((rand() % 1800) + 300), static_cast<float>((rand() % 500)));

    return true;
}

void BowserView::UpdateInput(float delta)
{
    if (m_canFire && m_collidingWithPlayer)
        m_pGameLayer->GetEventManager().QueueEvent(
            std::make_unique<CreateActorAtLocationEvent>(
                GetActor()->GetId(), 
                Resource("Actors/BowserFire.xml"), 
                "BowserFire", 
                Vector2<float>(m_pActorTransform->GetPosition())));
    
    m_processManager.UpdateProcesses(delta);
}

void BowserView::ViewScene()
{
}

void BowserView::Delete()
{
    auto pGameLayer = Bel::ApplicationLayer::GetInstance()->GetGameLayer();

    for (auto [guid, id] : m_listenerIds)
    {
        pGameLayer->GetEventManager().RemoveEventListener(guid, id);
    }


    m_processManager.AbortAllProcesses();
}

void BowserView::OnCollisionEnter(Collision& col)
{
    Actor* pOther = col.m_pActor;

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

void BowserView::OnTriggerEnter(Collision& col)
{
    Actor* pOther = col.m_pActor;

    if (pOther->IsName("Mario"))
    {
        m_collidingWithPlayer = true;
    }
}

void BowserView::OnTriggerExit(Collision& col)
{
    Actor* pOther = col.m_pActor;

    if (pOther->IsName("Mario"))
    {
        m_collidingWithPlayer = false;
    }
}

void BowserView::SpawnProjectile(Bel::IEvent* pEvent)
{
    auto pSpawnProjectile = static_cast<CreateActorAtLocationEvent*>(pEvent);

    Actor::Id actorId = pSpawnProjectile->GetActorId();
    const Vector2<float>& kPosToSpawn = pSpawnProjectile->GetPositionToSpawn();
    auto pActor = ApplicationLayer::GetInstance()->GetGameLayer()->GetActor(actorId);
    if (!pActor || GetActor()->GetId() != actorId)
    {
        return;
    }

    m_canFire = false;

    // Create bullet object, and bullet view.
    auto pGameLayer     = ApplicationLayer::GetInstance()->GetGameLayer();
    auto pResource      = pGameLayer->GetResourceCache()->GetHandle(&pSpawnProjectile->GetResource());
    auto pProjectile    = pGameLayer->GetActorFactory().CreateActorByResource(pResource);

    pProjectile->SetName(pSpawnProjectile->GetActorName());

    auto pProjectileBody = static_cast<Bel::IDynamicBodyComponent*>(pProjectile->GetComponent(kDynamicBodyId));
    if (!pProjectileBody)
    {
        LOG_FATAL("Failed to find IDynamicBodyComponent fromt " + pProjectile->GetName(), false);
        LOG_FATAL(". Message from 'SpawnProjectile() in BowserView'");
        return;
    }

    auto pProjectileTransform = static_cast<Bel::TransformComponent*>(pProjectile->GetComponent(kTransformId));
    if (!pProjectileTransform)
    {
        LOG_FATAL("Failed to find TransformComponent fromt " + pProjectile->GetName(), false);
        LOG_FATAL(". Message from 'SpawnProjectile() in BowserView'");
        return;
    }

    m_pAudioSource->PlayClip("Bowser_Fire");
    const unsigned int kAdjustX = 15;
    const unsigned int kAdjustY = (rand() % 30) + 15;

    pProjectileBody->SetPosition(kPosToSpawn.m_x - kAdjustX, kPosToSpawn.m_y + kAdjustY);

    pGameLayer->AddActor(pProjectile->GetId(), pProjectile);

    auto pCooldown = std::make_shared<DelayProcess>(3.0f);
    pCooldown->SetSuccessCallback
    (
        [this]()
        {
            m_canFire = true;
        }
    );
    m_processManager.AttachProcess(pCooldown);

    float xSpeed= static_cast<float>(pProjectileTransform->GetSpeed());
    auto pProjectileMove = std::make_shared<MoveConstantlyForSecondsProcess>(pProjectile, 6.f, xSpeed, 0.f);
    m_processManager.AttachProcess(pProjectileMove);
}

void BowserView::RemoveAll(Bel::IEvent* pEvent)
{
    m_processManager.AbortAllProcesses();
}

void BowserView::DeleteActorAndView()
{
    ApplicationLayer::GetInstance()->GetGameLayer()->GetEventManager().QueueEvent(std::make_unique<DestroyActorEvent>(GetActor()->GetId()));
    ApplicationLayer::GetInstance()->GetGameLayer()->GetEventManager().QueueEvent(std::make_unique<RemoveViewEvent>(this));
}

void BowserView::ReachGoal(Bel::IEvent* pEvent)
{
    Delete();
    ApplicationLayer::GetInstance()->GetGameLayer()->GetEventManager().QueueEvent(std::make_unique<RemoveViewEvent>(this));
}
