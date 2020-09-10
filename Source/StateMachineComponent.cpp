#include <Core/Layers/ApplicationLayer.h>
#include "StateMachineComponent.h"
#include "Scripting/Scripting.h"

using namespace tinyxml2;
using namespace Bel;

//#include " .gen"


bool IStateMachineComponent::Initialize(tinyxml2::XMLElement* pData)
{
    return true;
}

bool IStateMachineComponent::PostInit()
{
    return true;
}

void IStateMachineComponent::Update(float delta)
{
}


void IStateMachineComponent::ChangeState(const char* pName)
{
    auto pNewState = m_states.find(pName);
    if (pNewState != m_states.end())
    {
        m_pRootState = pNewState->second.get();
    }
    else if(pNewState == m_states.end())
    {
        m_pRootState->ChangeState(pName);
    }
}

void IStateMachineComponent::ParseSubStateData(tinyxml2::XMLElement* pData, StateMap& states)
{
    std::unique_ptr<IState> pState;
    for (XMLElement* pElement = pData->FirstChildElement(); pElement; pElement = pElement->NextSiblingElement())
    {
        const char* pName = pElement->Name();
        auto createItr = m_stateCreatorMap.find(pName);

        if (createItr != m_stateCreatorMap.end())
        {
            pState = std::move(createItr->second(GetOwner()));
        }

        if (pState == nullptr)
        {
            return;
        }

        if (XMLElement* pSub = pElement->FirstChildElement(); pSub != nullptr)
        {
            ParseSubStateData(pElement, pState->GetSubStates());
        }
        
        if (!pState->Initialize(pElement))
        {
            return;
        }

        const char* pStateName = pElement->Attribute("name");
        if (!pStateName)
        {
            pStateName = pName;
            pState->SetName(pName);
        }

        const char* pInit = pElement->Attribute("initState");
        if(pInit)
            pState->ChangeState(pInit);


        states.emplace(pStateName, std::move(pState));
    }
}

void IState::AddSubState(const char* pName, std::unique_ptr<IState> pSubState)
{
    m_subStates.emplace(pName, std::move(pSubState));
}

void IState::ChangeState(const char* pName)
{
    if (m_subStates.empty())
        return;

    auto pNewState = m_subStates.find(pName);
    if (pNewState != m_subStates.end())
    {
        m_pCurrentState = pNewState->second.get();
    }
    else if (pNewState == m_subStates.end())
    {
        m_pCurrentState->ChangeState(pName);
    }
}

bool IState::InitSubStates(XMLElement* pData)
{
    for (StateMap::const_iterator pState = m_subStates.begin(); pState != m_subStates.end(); ++pState)
    {
        if (!pState->second->Initialize(pData))
            return false;
    }

    return true;
}

//void IStateMachineComponent::RegisterWithScript(Bel::ScriptingManager& scripting)
//{
//    scripting.GetGlobal("g_logic");
//    scripting.GetFromTable("Actors");
//    scripting.GetFromTable(GetOwner()->GetId());
//
//    scripting.CreateTable();
//    scripting.AddToTable("this", this);
//    scripting.AddToTable("ChangeState", Lua::StateMachineChangeState);
//    scripting.AddToTable("StateMachine");
//
//    scripting.PopAll();
//}