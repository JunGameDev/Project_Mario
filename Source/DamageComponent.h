#pragma once
#include <Actors/Actor.h>

class DamageComponent : public Bel::IActorComponent
{
private:
    unsigned int m_damagePoint;

public:
    DamageComponent(Bel::Actor* pOwner, const char* pName);
    virtual ~DamageComponent() override;
    // Inherited via IActorComponent
    virtual bool Initialize(tinyxml2::XMLElement* pData) override;

    LUA_REGISTER();
    LUA_GETGLUE(GetDamagePoint, int, damagePoint);
};

