#include <Core/Layers/ApplicationLayer.h>
#include "DamageComponent.h"
using namespace Bel;


#include "DamageComponent.gen"

DamageComponent::DamageComponent(Bel::Actor* pOwner, const char* pName)
    : IActorComponent(pOwner, pName)
    , m_damagePoint(0)
{
}

DamageComponent::~DamageComponent()
{
}

bool DamageComponent::Initialize(tinyxml2::XMLElement* pData)
{
    if (pData)
    {
        m_damagePoint = pData->IntText();
    }

    return true;
}
