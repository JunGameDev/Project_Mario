#include "TransformComponent.h"
#include "Game.h"

using namespace Bel;


/****************************************************************
 *                      TransformComponent
 ***************************************************************/
bool TransformComponent::Initialize(tinyxml2::XMLElement* pData)
{
    auto pElement = pData->FirstChildElement("Position");
    if (pElement != nullptr)
    {
        m_x = pElement->FloatAttribute("x");
        m_y = pElement->FloatAttribute("y");
    }

    pElement = pData->FirstChildElement("Movement");
    if (pElement != nullptr)
    {
        m_speed = pElement->IntAttribute("speed");
    }
    return true;
}