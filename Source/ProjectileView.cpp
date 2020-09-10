#include <Core/Layers/ApplicationLayer.h>

#include "ProjectileView.h"
#include "GameProcess.h"
#include "GameEvents.h"
#include "ComponentIDs.h"
#include "HealthComponent.h"

using namespace Bel;

bool ProjectileView::Initialize()
{
    m_pTransform = static_cast<Bel::TransformComponent*>(GetActor()->GetComponent(kTransformId));
    if (m_pTransform == nullptr)
    {
        LOG_FATAL("Failed to acquire TransformComponent from " + GetActor()->GetName());
        return false;
    }

    return true;
}

void ProjectileView::UpdateInput(float delta)
{
    m_processManager.UpdateProcesses(delta);
}

void ProjectileView::ViewScene()
{
}

void ProjectileView::Delete()
{
    auto pGameLayer = Bel::ApplicationLayer::GetInstance()->GetGameLayer();
    for (auto [guid, id] : m_listenerIds)
    {
        pGameLayer->GetEventManager().RemoveEventListener(guid, id);
    }

    m_processManager.AbortAllProcesses();
}

void ProjectileView::RemoveAll(Bel::IEvent* pEvent)
{
    m_processManager.AbortAllProcesses();
}
