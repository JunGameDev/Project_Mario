#include <Core/Layers/ApplicationLayer.h>
#include "HealthComponent.h"

using namespace Bel;
#include "HealthComponent.gen"

HealthComponent::HealthComponent(Bel::Actor* pOwner, const char* pName)
    : IActorComponent(pOwner, pName)
{

}

HealthComponent::~HealthComponent()
{
}

bool HealthComponent::Initialize(tinyxml2::XMLElement* pData)
{
    if (pData)
    {
        m_hp = pData->IntText();
    }

    return true;
}