#pragma once

#include <View.h>
#include <memory>
#include <process.h>
#include <Actor.h>


class TransformComponent;
class LightView : public Bel::IView
{
    const static uint32_t kNumPositions = 2;

private:
    std::shared_ptr<Bel::IProcess> m_pMoveProcess;
    std::shared_ptr<Bel::IProcess> m_pDelayProcess;

    TransformComponent* m_pTransform;

    Bel::ProcessManager m_processManager;
    float m_positions[kNumPositions];
    int32_t m_currentPosition;

    Bel::IMouse* m_pMouse;

public:
    LightView(int currentPosition)
        : m_positions
            {
                30.0f, 200.0f,
            }
        , m_currentPosition(currentPosition)
    {
    }
    virtual ~LightView() override {}
    virtual bool Initialize() override;
    virtual void UpdateInput(float delta) override;
    virtual void ViewScene() override {}

private:
    void CreateMoveProcess();
    void CreateDelayProcess();
};