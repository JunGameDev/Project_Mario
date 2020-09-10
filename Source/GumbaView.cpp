#include <Core/Layers/ApplicationLayer.h>

#include "GumbaView.h"
#include "ComponentIDs.h"
#include "AnimationComponent.h"
#include "GameProcess.h"

using namespace Bel;

bool GumbaView::Initialize()
{
    // --- Actor Components ---
    {
        if (!GetActor()->HasComponent(kTransformId))
        {
            LOG_FATAL("Unable to get TransformComponent from Gumba");
            return false;
        }

        m_pTransform = static_cast<TransformComponent*>(GetActor()->GetComponent(kTransformId));
        if (!m_pTransform)
        {
            LOG_FATAL("Unable to get AnimationComponent from Gumba");
            return false;
        }

        m_pAnim = static_cast<AnimationComponent*>(GetActor()->GetComponent(kAnimationId));
        if (!m_pAnim)
        {
            LOG_FATAL("Unable to get AnimationComponent from Gumba");
            return false;
        }
        m_pAnim->SetAnimation("Walk");


        m_pBody = static_cast<IDynamicBodyComponent*>(GetActor()->GetComponent(kDynamicBodyId));
        if (m_pBody == nullptr)
        {
            LOG_FATAL("Unable to get IDynamicBodyComponent from Gumba");
            return false;
        }
    }

    m_pPatrolProcess = std::make_shared<NewPatrolProcess>(GetActor(), NewPatrolProcess::PatrolType::kCollision, "Processes/HorizontalCollisionPatrol.xml", false);
    m_processManager.AttachProcess(m_pPatrolProcess);

    return true;
}

void GumbaView::UpdateInput(float delta)
{
    m_processManager.UpdateProcesses(delta);
}

void GumbaView::ViewScene()
{
}

void GumbaView::Delete()
{
    auto pGameLayer = Bel::ApplicationLayer::GetInstance()->GetGameLayer();
    for (auto [guid, id] : m_listenerIds)
    {
        pGameLayer->GetEventManager().RemoveEventListener(guid, id);
    }

    m_processManager.AbortAllProcesses();
}

void GumbaView::OnCollisionEnter(Collision& col)
{
    Actor* pOther = col.m_pActor;

    if (pOther->IsName("Wall"))
    {
        if (m_pPatrolProcess != nullptr)
            m_pPatrolProcess->Aborted();

        m_pPatrolProcess = std::make_shared<NewPatrolProcess>(GetActor(), NewPatrolProcess::PatrolType::kCollision, "Processes/HorizontalCollisionPatrol.xml", m_movingRight);
        m_processManager.AttachProcess(m_pPatrolProcess);
        m_movingRight = !m_movingRight;
    }
}

void GumbaView::OnTriggerEnter(Collision& col)
{

}