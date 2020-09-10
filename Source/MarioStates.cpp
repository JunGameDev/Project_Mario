#include <Core/Layers/ApplicationLayer.h>

#include "Game.h"
#include "MarioStates.h"
#include "ComponentIDs.h"

#include "TextureComponent.h"
#include "AnimationComponent.h"

using namespace tinyxml2;
using namespace Bel;

namespace Lua
{
    static int StateMachineChangeState(lua_State* pState)
    {
        auto pStateMachine = reinterpret_cast<IStateMachineComponent*>(lua_touserdata(pState, 1));
        const char* pStateName = static_cast<const char*>(lua_tostring(pState, 2));

        pStateMachine->ChangeState(pStateName);
        pStateMachine->HandleInput();

        lua_pop(pState, 2);
        return 0;
    }

    static int StateMachineGetStateName(lua_State* pState)
    {
        auto pStateMachine = reinterpret_cast<IStateMachineComponent*>(lua_touserdata(pState, 1));
        lua_pop(pState, 1);
        lua_pushstring(pState, pStateMachine->GetCurrentStateName().c_str());
        return 1;
    }
}

//=============================================================================
// Small State
//=============================================================================
bool SmallState::Initialize(XMLElement* pData)
{
    if (!InitSubStates(pData))
        return false;

    const char* pInitialState = pData->Attribute("InitState");
    m_pCurrentState = m_subStates.find(pInitialState)->second.get();
    
    return true;
}

bool SmallState::PostInit()
{
    for (auto& subState : GetSubStates())
    {
        if (!subState.second->PostInit())
            return false;
    }

    return true;
}

void SmallState::HandleInput()
{
    if (!m_pCurrentState)
        return;

    m_pCurrentState->HandleInput();
}

//=============================================================================
// Super State
//=============================================================================
bool SuperState::Initialize(XMLElement* pData)
{
    if (!InitSubStates(pData))
        return false;

    const char* pInitialState = pData->Attribute("InitState");
    m_pCurrentState = m_subStates.find(pInitialState)->second.get();

    return true;
}

bool SuperState::PostInit()
{
    for (auto& subState : GetSubStates())
    {
        if (!subState.second->PostInit())
            return false;
    }
    return true;
}

void SuperState::HandleInput()
{
    if (!m_pCurrentState)
        return;

    m_pCurrentState->HandleInput();
}

//=============================================================================
// SmallWalking State
//=============================================================================
bool SmallWalkingState::Initialize(XMLElement* pData)
{
    m_pAnimation = static_cast<AnimationComponent*>(m_pOwner->GetComponent(kAnimationId));
    if (!m_pAnimation)
    {
        LOG_FATAL("Failed to get AnimationComponent: Mario Small State");
        return false;
    }

    m_pTexture = static_cast<TextureComponent*>(m_pOwner->GetComponent(kTextureId));
    if (!m_pTexture)
    {
        LOG_FATAL("Failed to get TextureComponent: Mario Small State");
        return false;
    }


    m_pKeyboard = ApplicationLayer::GetInstance()->GetKeyboardInput();
    m_pPhysicsManager = &ApplicationLayer::GetInstance()->GetGameLayer()->GetPhysicsManager();
    return true;
}

bool SmallWalkingState::PostInit()
{
    m_pPlayerBody = static_cast<Bel::IDynamicBodyComponent*>(m_pOwner->GetComponent(kDynamicBodyId));
    if (!m_pPlayerBody)
    {
        LOG_FATAL("Failed to get IDynamicBodyComponent: Mario Small State");
        return false;
    }

    m_pTransform = static_cast<Bel::TransformComponent*>(m_pOwner->GetComponent(kTransformId));
    if (!m_pPlayerBody)
    {
        LOG_FATAL("Failed to get TransformComponent: Mario Small State");
        return false;
    }

    return true;
}

void SmallWalkingState::HandleInput()
{
    m_pAnimation->SetAnimation("Walk_Small");
    
    if (m_pKeyboard->IsKeyDown(IKeyboard::kArrowRight))
    {
        auto& scripting = ApplicationLayer::GetInstance()->GetGameLayer()->GetScriptingManager();
        scripting.StartFunction("MovePlayer");
        scripting.PushNumber(static_cast<double>(IKeyboard::kArrowRight));
        scripting.PushNumber(m_pOwner->GetId());
        scripting.CallFunction(0);

        scripting.StartFunction("UpdateCamera");
        scripting.CallFunction(0);

        m_pTexture->SetRenderFlip(RenderFlip::FLIP_NONE);
    }
    
    if (m_pKeyboard->IsKeyDown(IKeyboard::kArrowLeft))
    {
        auto& scripting = ApplicationLayer::GetInstance()->GetGameLayer()->GetScriptingManager();
        scripting.StartFunction("MovePlayer");
        scripting.PushNumber(static_cast<double>(IKeyboard::kArrowLeft));
        scripting.PushNumber(m_pOwner->GetId());
        scripting.CallFunction(0);

        scripting.StartFunction("UpdateCamera");
        scripting.CallFunction(0);

        m_pTexture->SetRenderFlip(RenderFlip::FLIP_HORIZONTAL);

    }
}

//=============================================================================
// SuperWalking State
//=============================================================================
bool SuperWalkingState::Initialize(XMLElement* pData)
{
    m_pAnimation = static_cast<AnimationComponent*>(m_pOwner->GetComponent(kAnimationId));
    if (!m_pAnimation)
    {
        LOG_FATAL("Failed to acquire AnimationComponent: Mario Small State");
        return false;
    }

    m_pTexture = static_cast<TextureComponent*>(m_pOwner->GetComponent(kTextureId));
    if (!m_pTexture)
    {
        LOG_FATAL("Failed to acquire TextureComponent: Mario Small State");
        return false;
    }

    m_pKeyboard = ApplicationLayer::GetInstance()->GetKeyboardInput();

    return true;
}

bool SuperWalkingState::PostInit()
{
    m_pPlayerBody = static_cast<Bel::IDynamicBodyComponent*>(m_pOwner->GetComponent(kDynamicBodyId));
    if (!m_pPlayerBody)
    {
        LOG_FATAL("Failed to get IDynamicBodyComponent: Mario Super State");
        return false;
    }

    m_pTransform = static_cast<Bel::TransformComponent*>(m_pOwner->GetComponent(kTransformId));
    if (!m_pPlayerBody)
    {
        LOG_FATAL("Failed to get TransformComponent: Mario Super State");
        return false;
    }

    return true;
}

void SuperWalkingState::HandleInput()
{
    m_pAnimation->SetAnimation("Walk_Super");

    if (m_pKeyboard->IsKeyDown(IKeyboard::kArrowRight))
    {
        auto& scripting = ApplicationLayer::GetInstance()->GetGameLayer()->GetScriptingManager();
        scripting.StartFunction("MovePlayer");
        scripting.PushNumber(static_cast<double>(IKeyboard::kArrowRight));
        scripting.PushNumber(m_pOwner->GetId());
        scripting.CallFunction(0);

        scripting.StartFunction("UpdateCamera");
        scripting.CallFunction(0);

        m_pTexture->SetRenderFlip(RenderFlip::FLIP_NONE);
    }

    if (m_pKeyboard->IsKeyDown(IKeyboard::kArrowLeft))
    {
        auto& scripting = ApplicationLayer::GetInstance()->GetGameLayer()->GetScriptingManager();
        scripting.StartFunction("MovePlayer");
        scripting.PushNumber(static_cast<double>(IKeyboard::kArrowLeft));
        scripting.PushNumber(m_pOwner->GetId());
        scripting.CallFunction(0);

        scripting.StartFunction("UpdateCamera");
        scripting.CallFunction(0);

        m_pTexture->SetRenderFlip(RenderFlip::FLIP_HORIZONTAL);
    }
}

//=============================================================================
// SuperIdle State
//=============================================================================
bool SuperIdleState::Initialize(XMLElement* pData)
{
    m_pAnimation = static_cast<AnimationComponent*>(m_pOwner->GetComponent(kAnimationId));
    if (!m_pAnimation)
    {
        LOG_FATAL("Failed to acquire AnimationComponent: Mario Small State");
        return false;
    }

    m_pTexture = static_cast<TextureComponent*>(m_pOwner->GetComponent(kTextureId));
    if (!m_pTexture)
    {
        LOG_FATAL("Failed to acquire TextureComponent: Mario Small State");

        return false;
    }


    return true;
}

bool SuperIdleState::PostInit()
{
    m_pPlayerBody = static_cast<Bel::IDynamicBodyComponent*>(m_pOwner->GetComponent(kDynamicBodyId));
    if (!m_pPlayerBody)
    {
        LOG_FATAL("Failed to get IDynamicBodyComponent: Mario Small State");
        return false;
    }

    return true;
}

void SuperIdleState::HandleInput()
{
    m_pAnimation->SetAnimation("Idle_Super");
    //m_pPlayerBody->SetDimensions(20, 14, "Head");
}

//=============================================================================
// SmallIdle State
//=============================================================================
bool SmallIdleState::Initialize(XMLElement* pData)
{
    m_pAnimation = static_cast<AnimationComponent*>(m_pOwner->GetComponent(kAnimationId));
    if (!m_pAnimation)
    {
        LOG_FATAL("Failed to acquire AnimationComponent: Mario Small State");
        return false;
    }

    m_pTexture = static_cast<TextureComponent*>(m_pOwner->GetComponent(kTextureId));
    if (!m_pTexture)
    {
        LOG_FATAL("Failed to acquire TextureComponent: Mario Small State");
        return false;
    }
    m_pKeyboard = ApplicationLayer::GetInstance()->GetKeyboardInput();

    return true;
}

bool SmallIdleState::PostInit()
{
    m_pPlayerBody = static_cast<Bel::IDynamicBodyComponent*>(m_pOwner->GetComponent(kDynamicBodyId));
    if (!m_pPlayerBody)
    {
        LOG_FATAL("Failed to get IDynamicBodyComponent: Mario Small State");
        return false;
    }
    return true;
}

void SmallIdleState::HandleInput()
{
    m_pAnimation->SetAnimation("Idle_Small");
}

static std::unique_ptr<IState> CreateSmallState(Actor* pActor)
{
    return std::make_unique<SmallState>(pActor);
}

static std::unique_ptr<IState> CreateSmallWalkingState(Actor* pActor)
{
    return std::make_unique<SmallWalkingState>(pActor);
}

static std::unique_ptr<IState> CreateSmallIdleState(Actor* pActor)
{
    return std::make_unique<SmallIdleState>(pActor);
}

static std::unique_ptr<IState> CreateSuperState(Actor* pActor)
{
    return std::make_unique<SuperState>(pActor);
}

static std::unique_ptr<IState> CreateSuperWalkingState(Actor* pActor)
{
    return std::make_unique<SuperWalkingState>(pActor);
}

static std::unique_ptr<IState> CreateSuperIdleState(Actor* pActor)
{
    return std::make_unique<SuperIdleState>(pActor);
}


//=============================================================================
// State Machine
//=============================================================================
bool MarioStateMachine::Initialize(tinyxml2::XMLElement* pData)
{
    RegisterStateCreator("SmallState",           &CreateSmallState);
    RegisterStateCreator("SmallIdleState",       &CreateSmallIdleState);
    RegisterStateCreator("SmallWalkingState",    &CreateSmallWalkingState);

    RegisterStateCreator("SuperState",           &CreateSuperState);
    RegisterStateCreator("SuperIdleState",       &CreateSuperIdleState);
    RegisterStateCreator("SuperWalkingState",    &CreateSuperWalkingState);

    ParseSubStateData(pData, m_states);
    
    const char* pInitialState = pData->Attribute("InitState");
    if (pInitialState)
        m_pRootState = m_states.find(pInitialState)->second.get();

    return true;
}

bool MarioStateMachine::PostInit()
{
    for (auto& state : GetAllStates())
    {
        if (!state.second->PostInit())
            return false;
    }

    HandleInput();
    return true;
}

void MarioStateMachine::Update(float delta)
{
}

void MarioStateMachine::RegisterWithScript()
{
    auto& scripting = ApplicationLayer::GetInstance()->GetGameLayer()->GetScriptingManager();
    scripting.GetGlobal("g_logic");
    scripting.GetFromTable("actors");
    scripting.GetFromTable(GetOwner()->GetId());

    scripting.CreateTable();
    scripting.AddToTable("this", this);
    scripting.AddToTable("ChangeState", Lua::StateMachineChangeState);
    scripting.AddToTable("GetStateName", Lua::StateMachineGetStateName);
    scripting.AddToTable("StateMachineComponent");
    scripting.PopAll();
}