#pragma once

#include <Actors/Actor.h>

class HealthComponent : public Bel::IActorComponent
{
private:
    unsigned int m_hp;
public:
    HealthComponent(Bel::Actor* pOwner, const char* pName);
    
    virtual ~HealthComponent() override;

    LUA_REGISTER();
    LUA_TAKEGLUE(TakeDamage, int, hp);
    LUA_GAINGLUE(GainPoint, int, hp);
    LUA_GETGLUE(GetHP, int, hp);
    LUA_SETGLUE(SetHP, int, hp);

    // Inherited via IActorComponent
    virtual bool Initialize(tinyxml2::XMLElement* pData) override;
    virtual bool PostInit() { return true; }
    virtual void Update(float delta) {}
};

