#pragma once
#include <memory>
#include <Actors/Actor.h>

#include "BoxRenderComponent.h"
#include "TextureComponent.h"
#include "AudioSource.h"
#include "AnimationComponent.h"
#include "MarioStates.h"
#include "ButtonComponent.h"
#include "UIImage.h"
#include "ParticleComponent.h"
#include "TagComponent.h"
#include "HealthComponent.h"
#include "Attackable.h"
#include "DamageComponent.h"
#include "ActorType.h"
#include "CrateComponent.h"

using namespace Bel;

static std::unique_ptr<IActorComponent> CreateBoxRenderComponent(Bel::Actor* pOwner, const char* pName)
{
    return std::unique_ptr<IActorComponent>(new BoxRenderComponent(pOwner, pName));
}
static std::unique_ptr<IActorComponent> CreateTextureComponent(Bel::Actor* pOwner, const char* pName)
{
    return std::unique_ptr<IActorComponent>(new TextureComponent(pOwner, pName));
}

static std::unique_ptr<IActorComponent> CreateAudioSourceComponent(Bel::Actor* pOwner, const char* pName)
{
    return std::unique_ptr<IActorComponent>(new AudioSource(pOwner, pName));
}

static std::unique_ptr<IActorComponent> CreateAnimationComponent(Bel::Actor* pOwner, const char* pName)
{
    return std::unique_ptr<IActorComponent>(new AnimationComponent(pOwner, pName));
}

static std::unique_ptr<IActorComponent> CreateMarioStateComponent(Bel::Actor* pOwner, const char* pName)
{
    return std::unique_ptr<IActorComponent>(new MarioStateMachine(pOwner, pName));
}

static std::unique_ptr<IActorComponent> CreateButtonComponent(Bel::Actor* pOwner, const char* pName)
{
    return std::unique_ptr<IActorComponent>(new ButtonComponent(pOwner, pName));
}

static std::unique_ptr<IActorComponent> CreateUIImage(Bel::Actor* pOwner, const char* pName)
{
    return std::unique_ptr<IActorComponent>(new UIImage(pOwner, pName));
}

static std::unique_ptr<IActorComponent> CreateParticleComponent(Bel::Actor* pOwner, const char* pName)
{
    return std::unique_ptr<IActorComponent>(new ParticleComponent(pOwner, pName));
}

static std::unique_ptr<IActorComponent> CreateTagComponent(Bel::Actor* pOwner, const char* pName)
{
    return std::unique_ptr<IActorComponent>(new TagComponent(pOwner, pName));
}

static std::unique_ptr<IActorComponent> CreateHealthComponent(Bel::Actor* pOwner, const char* pName)
{
    return std::unique_ptr<IActorComponent>(new HealthComponent(pOwner, pName));
}

static std::unique_ptr<IActorComponent> CreateAttackable(Bel::Actor* pOwner, const char* pName)
{
    return std::unique_ptr<IActorComponent>(new Attackable(pOwner, pName));
}

static std::unique_ptr<IActorComponent> CreateDamageComponent(Bel::Actor* pOwner, const char* pName)
{
    return std::unique_ptr<IActorComponent>(new DamageComponent(pOwner, pName));
}

static std::unique_ptr<IActorComponent> CreateCrateComponent(Bel::Actor* pOwner, const char* pName)
{
    return std::unique_ptr<IActorComponent>(new CrateComponent(pOwner, pName));
}