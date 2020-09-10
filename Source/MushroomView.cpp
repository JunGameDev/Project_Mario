#include <Core/Layers/ApplicationLayer.h>

#include "MushroomView.h"
#include "ComponentIDs.h"
#include "AnimationComponent.h"
#include "GameProcess.h"

using namespace Bel;

bool MushroomView::Initialize()
{
    // --- Actor Components ---
    {
        m_pTransform = static_cast<TransformComponent*>(GetActor()->GetComponent(kTransformId));
        if (!m_pTransform)
        {
            LOG_FATAL("Unable to get TransformComponent from Mushroom");
            return false;
        }

        m_pBody = static_cast<IDynamicBodyComponent*>(GetActor()->GetComponent(kDynamicBodyId));
        if (m_pBody == nullptr)
        {
            LOG_FATAL("Unable to get IDynamicBodyComponent from Mushroom");
            return false;
        }
    }

    m_pPatrolProcess = std::make_shared<NewPatrolProcess>(GetActor(), NewPatrolProcess::PatrolType::kCollision, "Processes/HorizontalCollisionPatrol.xml", m_movingRight);
    m_processManager.AttachProcess(m_pPatrolProcess);

    return true;
}

void MushroomView::UpdateInput(float delta)
{
    m_processManager.UpdateProcesses(delta);
}

void MushroomView::ViewScene()
{
}

void MushroomView::Delete()
{
    auto pGameLayer = ApplicationLayer::GetInstance()->GetGameLayer();
    for (auto [guid, id] : m_listenerIds)
    {
        pGameLayer->GetEventManager().RemoveEventListener(guid, id);
    }

    m_processManager.AbortAllProcesses();
}

void MushroomView::OnCollisionEnter(Collision& col)
{
    Actor* pOther = col.m_pActor;

    if (pOther->IsName("Wall"))
    {
        if (m_pPatrolProcess != nullptr)
            m_pPatrolProcess->Aborted();

        m_movingRight = !m_movingRight;
        m_pPatrolProcess = std::make_shared<NewPatrolProcess>(GetActor(), NewPatrolProcess::PatrolType::kCollision, "Processes/HorizontalCollisionPatrol.xml", m_movingRight);
        m_processManager.AttachProcess(m_pPatrolProcess);
    }
}

void MushroomView::OnTriggerEnter(Collision& col)
{
}