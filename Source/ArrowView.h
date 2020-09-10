#pragma once

#include <View.h>
#include <Processes.h>
#include <Events.h>

class TransformComponent;

class ArrowView : public Bel::IView
{
private:
    TransformComponent* m_pTransform;
    std::size_t m_listenerId;
    bool m_canMove;
    Bel::ProcessManager m_processManager;
    bool m_headToRight;
    
    std::shared_ptr<Bel::IProcess> m_pLifeTime;

public:
    ArrowView(const bool& headToRight)
        : m_headToRight(headToRight)
    {
    }

    virtual ~ArrowView() override 
    {
        m_processManager.AbortAllProcesses();
    }

    virtual bool Initialize() override;
    virtual void UpdateInput(float delta) override;
    virtual void ViewScene() override {}

    virtual void OnCollisionEnter(Bel::Actor* pOther, bool isSensor = false) override;
    virtual void OnCollisionExit(Bel::Actor* pOther, bool isSensor = false) override;

    void RemoveAll(Bel::IEvent* pEvent);
};
