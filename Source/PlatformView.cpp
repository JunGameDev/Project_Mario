#include <Core/Layers/ApplicationLayer.h>

#include "PlatformView.h"
#include "GameProcess.h"

using namespace Bel;

//=====================================================================================================================
//                                              UpPlatformView
//=====================================================================================================================
bool UpPlatformView::Initialize()
{
    m_pPatrolProcess = std::make_shared<NewPatrolProcess>(GetActor(), "Processes/ElevatePatrolProcess.xml");

    m_processManager.AttachProcess(m_pPatrolProcess);

    return true;
}

void UpPlatformView::UpdateInput(float delta)
{
    m_processManager.UpdateProcesses(delta);
}

void UpPlatformView::ViewScene()
{
}

void UpPlatformView::Delete()
{
    auto pGameLayer = Bel::ApplicationLayer::GetInstance()->GetGameLayer();
    for (auto [guid, id] : m_listenerIds)
    {
        pGameLayer->GetEventManager().RemoveEventListener(guid, id);
    }

    m_processManager.AbortAllProcesses();
}

//=====================================================================================================================
//                                              DownPlatformView
//=====================================================================================================================
bool DownPlatformView::Initialize()
{
    m_pPatrolProcess = std::make_shared<NewPatrolProcess>(GetActor(), "Processes/DepressPlatformPatrolProcess.xml");
    m_processManager.AttachProcess(m_pPatrolProcess);

    return true;
}

void DownPlatformView::UpdateInput(float delta)
{
    m_processManager.UpdateProcesses(delta);
}

void DownPlatformView::ViewScene()
{
}

void DownPlatformView::Delete()
{
    auto pGameLayer = Bel::ApplicationLayer::GetInstance()->GetGameLayer();
    for (auto [guid, id] : m_listenerIds)
    {
        pGameLayer->GetEventManager().RemoveEventListener(guid, id);
    }

    m_processManager.AbortAllProcesses();
}
