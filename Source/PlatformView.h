// Copyright (c) 2020 Junyoung Kim

#pragma once
#include <Core/Layers/View.h>
#include <Events/Processes.h>

class UpPlatformView : public Bel::IView
{
private:
    Bel::ProcessManager m_processManager;
    std::shared_ptr<Bel::IProcess> m_pPatrolProcess;

public:
    UpPlatformView() {}
    virtual ~UpPlatformView() override
    {
        auto pGameLayer = Bel::ApplicationLayer::GetInstance()->GetGameLayer();
        for (auto [guid, id] : m_listenerIds)
        {
            pGameLayer->GetEventManager().RemoveEventListener(guid, id);
        }

        m_processManager.AbortAllProcesses();
    }
    
    virtual bool Initialize() override;
    virtual void UpdateInput(float delta) override;
    virtual void ViewScene() override;
    virtual void Delete() override;
};

class DownPlatformView : public Bel::IView
{
private:
    Bel::ProcessManager m_processManager;
    std::shared_ptr<Bel::IProcess> m_pPatrolProcess;

public:
    DownPlatformView() {}
    virtual ~DownPlatformView() override
    {
        auto pGameLayer = Bel::ApplicationLayer::GetInstance()->GetGameLayer();
        for (auto [guid, id] : m_listenerIds)
        {
            pGameLayer->GetEventManager().RemoveEventListener(guid, id);
        }

        m_processManager.AbortAllProcesses();

    }

    virtual bool Initialize() override;
    virtual void UpdateInput(float delta) override;
    virtual void ViewScene() override;
    virtual void Delete() override;
};

