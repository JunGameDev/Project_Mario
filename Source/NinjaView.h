#pragma once

#include <View.h>
#include <memory>
#include <process.h>
#include <Actor.h>

class ParticleComponent;
class ButtonComponent;

class NinjaView : public Bel::IView
{
    const static uint32_t kNumPositions = 8;
    const static uint32_t kNumDimensions = 2;

private:
    std::shared_ptr<Bel::IProcess> m_pMoveProcess;
    std::shared_ptr<Bel::IProcess> m_pDelayProcess;

    std::shared_ptr<Bel::IProcess> m_pPatrolProcess;
    std::shared_ptr<Bel::IProcess> m_pJumpProcess;
    std::shared_ptr<Bel::IProcess> m_pConfettiProcess;

    Bel::ProcessManager m_processManager;
    ParticleComponent* m_pParticle;
    float m_positions[kNumPositions][kNumDimensions];
    int32_t m_currentPosition;

    ButtonComponent* m_pStartDanceBtn;
    ButtonComponent* m_pStopDanceBtn;
    ButtonComponent* m_pConfettiBtn;

    Bel::IMouse* m_pMouse;

public:
    NinjaView()
        : m_positions
            {
                100.0f, 100.0f,
                200.0f, 0.0f,
                300.0f, 100.0f,
                400.0f, 200.0f,
                300.0f, 300.0f,
                200.0f, 400.0f,
                100.0f, 300.0f,
                0.0f, 200.0f
            }
        , m_currentPosition(0)
    {
    }
    virtual ~NinjaView() override {}
    virtual bool Initialize() override;
    virtual void UpdateInput(float delta) override;
    virtual void ViewScene() override {}

private:
    void CreateMoveProcess();
    void CreatePatrolProcess();
    void CreateDelayProcess();
    void CreateConfettiProcess();
    void CreateJumpProcess();
};