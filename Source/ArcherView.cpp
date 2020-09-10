#include <ApplicationLayer.h>

#include "ArcherView.h"
#include "TransformComponent.h"
#include "TextureComponent.h"
#include "AnimationComponent.h"
#include "AudioSource.h"
#include "ComponentIDs.h"
#include "ArrowView.h"
#include "Collider.h"

#include "GameEvents.h"
#include "GameProcess.h"

using namespace Bel;

bool ArcherView::Initialize()
{
    m_pGameLayer = ApplicationLayer::GetInstance()->GetGameLayer();
    if (!m_pGameLayer)
    {
        LOG_FATAL("Failed to acquire GameLayer.");
        return false;
    }

    auto pActor = GetActor();
    m_pActorTransform = static_cast<TransformComponent*>(pActor->GetComponent(kTransformId));
    if (!m_pActorTransform)
    {
        LOG_FATAL("Failed to find TransformComponent for actor id: ", false);
        LOG_FATAL(std::to_string(pActor->GetId()).c_str());
        return false;
    }

    m_pCollider = static_cast<BoxCollider2D*>(pActor->GetComponent(kBoxColliderId));
    if (!m_pCollider)
    {
        LOG_FATAL("Unable to acquire BoxCollider2D : Koopa Troopa");
        return false;
    }

    m_pRader = static_cast<RaderCollider*>(pActor->GetComponent(kRaderColliderId));
    if (!m_pRader)
    {
        LOG_FATAL("Unable to acquire RaderCollider : Koopa Troopa");
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
    m_pTexture->SetRenderFlip(RenderFlip::FLIP_HORIZONTAL);

    m_pActorAnim = static_cast<AnimationComponent*>(pActor->GetComponent(kAnimationId));
    if (!m_pActorAnim)
    {
        LOG_FATAL("Failed to find AnimationCompontn for actor id: ", false);
        LOG_FATAL(std::to_string(pActor->GetId()).c_str());
        return false;
    }

    m_pGameLayer->GetEventManager().AddEventListener(FireBulletEvent::kEventType, std::bind(&ArcherView::FireArrow, this, std::placeholders::_1));
    m_pGameLayer->GetEventManager().AddEventListener(ReachTheGoalEvent::kEventType, std::bind(&ArcherView::RemoveAll, this, std::placeholders::_1));


    m_pActorTransform->SetPosition(static_cast<float>(rand() % 2000), static_cast<float>(rand() % 2000));

    return true;
}

void ArcherView::UpdateInput(float delta)
{
    if (m_canFire && m_collidingWithPlayer)
        m_pGameLayer->GetEventManager().QueueEvent(std::make_unique<FireBulletEvent>("Actors/MagicalArrow.xml"));

    m_processManager.UpdateProcesses(delta);
}

void ArcherView::ViewScene()
{
#ifdef DEBUG
    m_pRader->RenderDebug();
#endif
}

void ArcherView::OnCollisionEnter(Bel::Actor* pOther, bool isSensor)
{
    switch (isSensor)
    {
    case true:
    {
        if (pOther->IsName("Mario"))
        {
            m_collidingWithPlayer = true;
        }
    }
        break;
    default:
        break;
    }

}

void ArcherView::OnCollisionExit(Bel::Actor* pOther, bool isSensor)
{
    switch (isSensor)
    {
    case true:
    {
        if (pOther->IsName("Mario"))
        {
            m_collidingWithPlayer = false;
        }
    }
    break;
    case false:
    {
    }
    break;
    }
}

void ArcherView::FireArrow(Bel::IEvent* pEvent)
{
    auto pFireBullet = static_cast<FireBulletEvent*>(pEvent);

    m_canFire = false;

    // Create bullet object, and bullet view.
    auto pGameLayer = ApplicationLayer::GetInstance()->GetGameLayer();
    auto pBullet = pGameLayer->GetActorFactory().CreateActor("Actors/MagicalArrow.xml");
    pBullet->SetName("Arrow");
    auto pBulletTransform = static_cast<TransformComponent*>(pBullet->GetComponent(kTransformId));

    if (!pBulletTransform)
    {
        LOG_FATAL("Failed to find TransformCompoenent fromt actor id: ", false);
        LOG_FATAL(std::to_string(pBullet->GetId()).c_str(), false);
        LOG_FATAL(" Called from 'FireArrow() in ArcherView'");

        return;
    }
    pBulletTransform->SetPosition(m_pActorTransform->GetX(), m_pActorTransform->GetY());

    pGameLayer->AddActor(pBullet->GetId(), pBullet);

    Bel::RenderFlip flip = m_pTexture->GetRenderFlip();
    bool headToRight = flip == RenderFlip::FLIP_HORIZONTAL ? false : true;

    auto pView = new ArrowView(headToRight);
    pView->SetActor(pBullet);

    if (!pView->Initialize())
    {
        return;
    }

    pGameLayer->AddView(std::unique_ptr<IView>(pView));

    // --- Audio clip ---
    m_pAudioSource->PlayClip("Arrow");

    auto pCooldown = std::make_shared<DelayProcess>(6.0f);
    pCooldown->SetSuccessCallback(
        [&]()
        {
            m_pActorAnim->SetAnimation("Idle");
            m_canFire = true;
        });
    m_processManager.AttachProcess(pCooldown);

    m_pActorAnim->SetAnimation("Attack");
}

void ArcherView::RemoveAll(Bel::IEvent* pEvent)
{
    m_processManager.AbortAllProcesses();

    ApplicationLayer::GetInstance()->GetGameLayer()->GetEventManager().QueueEvent(std::make_unique<DestroyViewEvent>(this));

}
