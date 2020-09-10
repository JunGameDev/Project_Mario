#pragma once
#include <Actors/Actor.h>

class CrateComponent : public Bel::IActorComponent
{
private:
    std::string m_itemName; // Name of the item to spawn
    bool m_isEmpty;
public:
    CrateComponent(Bel::Actor* pOwner, const char* pName)
        : Bel::IActorComponent(pOwner, pName)
        , m_isEmpty(false)
    {
    }

    virtual ~CrateComponent() override {}

    virtual bool Initialize(tinyxml2::XMLElement* pData) override;
    virtual bool PostInit() override;
    LUA_REGISTER();

    std::string& GetItemName() { return m_itemName; }
    bool IsEmpty() const { return m_isEmpty; }
    void Empty() { m_isEmpty = true; }
};

