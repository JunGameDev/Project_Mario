#pragma once

#include <Core/Layers/View.h>
#include <Events/Processes.h>
#include <Events/Events.h>
#include <Physics/Physics.h>

class ProjectileView : public Bel::IView
{
private:
    Bel::TransformComponent* m_pTransform;
    std::size_t m_listenerId;
    bool m_canMove;
    Bel::ProcessManager m_processManager;
    bool m_headToRight;
    
    std::shared_ptr<Bel::IProcess> m_pLifeTime;

public:
    ProjectileView()
    {
    }

    virtual ~ProjectileView() override 
    {
        m_processManager.AbortAllProcesses();
    }

    virtual bool Initialize() override;
    virtual void UpdateInput(float delta) override;
    virtual void ViewScene() override;
    virtual void Delete() override;

    virtual void OnCollisionEnter(Bel::Collision& col) override {}
    virtual void OnCollisionExit(Bel::Collision& col) override {}

    void RemoveAll(Bel::IEvent* pEvent);
};
