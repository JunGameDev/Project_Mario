#pragma once

#include <Actors/Actor.h>
#include <Graphics/Graphics.h>
#include <unordered_map>
#include <Input/Input.h>
#include <memory>
#include <functional>

class IState;

using StateMap = std::unordered_map<std::string, std::unique_ptr<IState>>;

class IState
{
public:

protected:
    StateMap m_subStates;
    Bel::Actor* m_pOwner;
    IState* m_pCurrentState;
    std::string m_stateName;

public:
    IState(Bel::Actor* pActor)
        : m_pOwner(pActor)
    {
    }
    virtual ~IState() = 0 {};
    virtual void HandleInput() = 0;
    virtual bool Initialize(tinyxml2::XMLElement* pData) = 0;
    virtual bool PostInit() = 0;

    void AddSubState(const char* pName, std::unique_ptr<IState> pSubState);
    void ChangeState(const char* pName);

    StateMap& GetSubStates() { return m_subStates; }
    const std::string& GetStateName() const { return m_stateName; }
    void SetName(const std::string& kName) { m_stateName = kName; }
protected:
    bool InitSubStates(tinyxml2::XMLElement* pData);
};

class IStateMachineComponent : public Bel::IActorComponent
{
public:
    typedef std::function<std::unique_ptr<IState>(Bel::Actor*)> StateFunction;

protected:
    IState* m_pRootState;
    StateMap m_states;
    std::unordered_map<std::string, StateFunction> m_stateCreatorMap;

public:
    IStateMachineComponent(Bel::Actor* pOwner, const char* pName)
        : IActorComponent(pOwner, pName)
    {
    }

    virtual ~IStateMachineComponent() override {}
    virtual bool Initialize(tinyxml2::XMLElement* pData) override;
    virtual bool PostInit() override;
    virtual void Update(float delta) override;
    virtual void Render(Bel::IGraphics* pGraphics) override {}
    
    //LUA_FUNCTIONGLUE_0(HandleInput, HandleInput);
    //LUA_FUNCTIONGLUE_1(ChangeState, ChangeState, char*, pStateName);

    void HandleInput() { m_pRootState->HandleInput(); }
    void ChangeState(const char* pName);
    StateMap& GetAllStates() { return m_states; }
    const std::string& GetCurrentStateName() const { return m_pRootState->GetStateName(); }

    void RegisterStateCreator(const char* pName, StateFunction pFunction)
    {
        m_stateCreatorMap[pName] = pFunction;
    }

protected:
    void ParseSubStateData(tinyxml2::XMLElement* pData, StateMap& states);
};
