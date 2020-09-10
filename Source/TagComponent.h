#pragma once
#include <Actors/Actor.h>

class TagComponent : public Bel::IActorComponent
{
private:
    std::string m_tag;

public:
    TagComponent(Bel::Actor* pOwner, const char* pName)
        : Bel::IActorComponent(pOwner, pName)
    {

    }
    virtual ~TagComponent() {}
    virtual bool Initialize(tinyxml2::XMLElement* pData) override;

    LUA_REGISTER();

    const std::string& GetTag() const { return m_tag; }
};