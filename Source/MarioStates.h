#pragma once

#include <Input/Input.h>
#include <Parshing/tinyxml2.h>
#include <Physics/Physics.h>

#include "StateMachineComponent.h"

class AnimationComponent;
class TextureComponent;
class BoxCollider2D;

class SmallState : public IState
{
public:
    SmallState(Bel::Actor* pActor)
        : IState(pActor)
    {
    }
    virtual ~SmallState() override 
    {
        m_subStates.clear();
    }

    // Inherited via IState
    virtual bool Initialize(tinyxml2::XMLElement* pData) override;
    virtual bool PostInit() override;
    virtual void HandleInput() override;
};

class SuperState : public IState
{
public:
    SuperState(Bel::Actor* pActor)
        : IState(pActor)
    {
    }
    virtual ~SuperState() override 
    {
        m_subStates.clear();
    }

    // Inherited via IState
    virtual bool Initialize(tinyxml2::XMLElement* pData) override;
    virtual bool PostInit() override;
    virtual void HandleInput() override;
};

class SuperWalkingState : public IState
{
private:
    TextureComponent* m_pTexture;
    AnimationComponent* m_pAnimation;
    Bel::IDynamicBodyComponent* m_pPlayerBody;
    Bel::TransformComponent* m_pTransform;

    Bel::IKeyboard* m_pKeyboard;
public:
    SuperWalkingState(Bel::Actor* pActor)
        : IState(pActor)
    {
    }
    virtual ~SuperWalkingState() override 
    {
        m_subStates.clear();
    }

    // Inherited via IState
    virtual bool Initialize(tinyxml2::XMLElement* pData) override;
    virtual bool PostInit() override;
    virtual void HandleInput() override;
};

class SuperIdleState : public IState
{
private:
    TextureComponent* m_pTexture;
    AnimationComponent* m_pAnimation;
    Bel::IDynamicBodyComponent* m_pPlayerBody;

public:
    SuperIdleState(Bel::Actor* pActor)
        : IState(pActor)
    {
    }
    virtual ~SuperIdleState() override 
    {
        m_subStates.clear();
    }

    // Inherited via IState
    virtual bool Initialize(tinyxml2::XMLElement* pData) override;
    virtual bool PostInit() override;
    virtual void HandleInput() override;
};

class SmallWalkingState : public IState
{
private:
    TextureComponent* m_pTexture;
    AnimationComponent* m_pAnimation;
    Bel::IDynamicBodyComponent* m_pPlayerBody;
    Bel::TransformComponent* m_pTransform;

    Bel::IKeyboard* m_pKeyboard;
    Bel::IPhysicsManager* m_pPhysicsManager;

public:
    SmallWalkingState(Bel::Actor* pActor)
        : IState(pActor)
    {
    }
    virtual ~SmallWalkingState() override 
    {
        m_subStates.clear();
    }

    // Inherited via IState
    virtual bool Initialize(tinyxml2::XMLElement* pData) override;
    virtual bool PostInit() override;
    virtual void HandleInput() override;
};

class SmallIdleState : public IState
{
private:
    TextureComponent* m_pTexture;
    AnimationComponent* m_pAnimation;
    Bel::IDynamicBodyComponent* m_pPlayerBody;
    Bel::IKeyboard* m_pKeyboard;

public:
    SmallIdleState(Bel::Actor* pActor)
        : IState(pActor)
    {
    }
    virtual ~SmallIdleState() override 
    {
        m_subStates.clear();
    }

    // Inherited via IState
    virtual bool Initialize(tinyxml2::XMLElement* pData) override;
    virtual bool PostInit() override;
    virtual void HandleInput() override;
};

class MarioStateMachine : public IStateMachineComponent
{
public:
    MarioStateMachine(Bel::Actor* pOwner, const char* pName)
        : IStateMachineComponent(pOwner, pName)
    {

    }
    virtual ~MarioStateMachine() override 
    {
        m_states.clear();
    }

    // Inherited via IActorComponent
    LUA_REGISTER();
    virtual bool Initialize(tinyxml2::XMLElement* pData) override;
    virtual bool PostInit() override;
    virtual void Update(float delta) override;
    virtual void Render(Bel::IGraphics* pGraphics) override {}
};