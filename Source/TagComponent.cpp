#include <Core/Layers/ApplicationLayer.h>
#include "TagComponent.h"

using namespace Bel;
#include "TagComponent.gen"

bool TagComponent::Initialize(tinyxml2::XMLElement* pData)
{
    m_tag = pData->GetText();

    return true;
}
