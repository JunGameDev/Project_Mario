#pragma once
#include <View.h>
#include <memory>
#include <Processes.h>

class ParticleComponent;

class DancerView : public Bel::IView
{
    const static uint32_t kNumPositions = 8;
    const static uint32_t kNumDimensions = 2;
private:
    std::shared_ptr<Bel::IProcess> m_pDelayProcess;
    std::shared_ptr<Bel::IProcess> m_pPatrolProcess;
    std::shared_ptr<Bel::IProcess> m_jumpProcess;
    std::shared_ptr<Bel::IProcess> m_pMoveProcess;

    Bel::ProcessManager m_processManager;
    ParticleComponent* m_pParticle;

    float m_positions[kNumPositions][kNumDimensions];
    int32_t m_currentPosition;

public:

    DancerView()
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
    {
        m_currentPosition = rand() % kNumPositions;
    }

    virtual ~DancerView() override {}
    virtual bool Initialize() override;
    virtual void UpdateInput(float delta) override
    {
        m_processManager.UpdateProcesses(delta);
    }
    virtual void ViewScene() override {}

private:
    void CreatePatrolProcess();
    void CreateDelayProcess();
    void CreateMoveProcess();
};
