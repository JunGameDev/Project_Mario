#pragma once
#include <Actors/Actor.h>

enum class ActorType
{
    kMario,
    kGumba,
    kKoopaTroopa,
    kBowser,
    kBox,
    kBowserFire,
    kSword,
    kBoots,
    kMushRoom,
    kFlower,
    kFireBall,
    kToad,
    kPrincessPeach,
    kTestForFileStatus,
    kCount
};

class PlayerComponent : public Bel::IActorComponent
{
public:
    PlayerComponent(Bel::Actor* pOwner, const char* pName)
        : Bel::IActorComponent(pOwner, pName)
    {
    }

    virtual bool Initialize(tinyxml2::XMLElement* pData) override { return true; }
};

class EnemyComponent : public Bel::IActorComponent
{
public:
    EnemyComponent(Bel::Actor* pOwner, const char* pName)
        : Bel::IActorComponent(pOwner, pName)
    {
    }

    virtual bool Initialize(tinyxml2::XMLElement* pData) override { return true; }
};


