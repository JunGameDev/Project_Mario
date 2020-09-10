#pragma once
#include <Actors/Actor.h>
#include <Graphics/Graphics.h>

class Attackable : public Bel::IActorComponent
{
private:
    bool m_canAttack;

public:
    Attackable(Bel::Actor* pOwner, const char* pName)
        : Bel::IActorComponent(pOwner, pName)
        , m_canAttack(false)
    {

    }
    virtual ~Attackable() {}
    virtual bool Initialize(tinyxml2::XMLElement* pData) override;
    virtual bool PostInit() override;
    virtual void Render(Bel::IGraphics* pGraphics) override;
    LUA_REGISTER();
    LUA_GETGLUE(CanAttack, bool, canAttack);
    LUA_SETGLUE(SetAttack, bool, canAttack);

    void ReadyToAttack()                        { m_canAttack = true; }
    void NotReadyToAttack()                     { m_canAttack = false; }
};