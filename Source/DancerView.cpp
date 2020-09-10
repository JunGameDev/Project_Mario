#include <ApplicationLayer.h>

#include "DancerView.h"
#include "GameProcess.h"

using namespace Bel;

bool DancerView::Initialize()
{
    CreateDelayProcess();
    m_processManager.AttachProcess(m_pDelayProcess);
    return true;
}

void DancerView::CreateDelayProcess()
{
    long time = rand() % 7 + 2;
    m_pDelayProcess = std::make_shared<DelayProcess>(time); // 3 seconds
    m_pDelayProcess->SetSuccessCallback(
        [this]()
        {
            CreateMoveProcess();
            m_pDelayProcess->AttachChild(m_pMoveProcess);
        });

    m_pDelayProcess->SetAbortCallback(
        [&]()
        {
            LOG_INFO("Delay Process: Aborted");
        });
}

void DancerView::CreatePatrolProcess()
{
    m_pPatrolProcess = std::make_shared<PatrolProcess>(GetActor(), "Processes/DancerPatrol.xml");
    m_pPatrolProcess->SetSuccessCallback(
        [this]()
        {
            CreateDelayProcess();
            m_pPatrolProcess->AttachChild(m_pDelayProcess);
        });
}

void DancerView::CreateMoveProcess()
{
    float time = static_cast<float>(rand() % 10 + 1);
    m_pMoveProcess = std::make_shared<MoveWithAnimProcess>(
        GetActor(),
        m_positions[m_currentPosition][0],
        m_positions[m_currentPosition][1],
        time);
    m_pMoveProcess->SetSuccessCallback(
        [this]()
        {
            CreateDelayProcess();
            m_pMoveProcess->AttachChild(m_pDelayProcess);
        });

    m_currentPosition = rand() % kNumPositions;
}
