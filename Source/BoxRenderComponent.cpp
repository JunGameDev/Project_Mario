#include <Core/Layers/ApplicationLayer.h>
#include <Physics/Physics.h>

#include "BoxRenderComponent.h"
#include "ComponentIDs.h"

using namespace Bel;

/****************************************************************
 *                      BoxRendererComponent
 ***************************************************************/
bool BoxRenderComponent::Initialize(tinyxml2::XMLElement* pData)
{
    auto pElement = pData->FirstChildElement("Color");
    if (pElement != nullptr)
    {
        m_color.m_r = pElement->UnsignedAttribute("r");
        m_color.m_g = pElement->UnsignedAttribute("g");
        m_color.m_b = pElement->UnsignedAttribute("b");
    }
    pElement = pData->FirstChildElement("Dimensions");
    if (pElement != nullptr)
    {
        m_width = pElement->IntAttribute("width");
        m_height = pElement->IntAttribute("height");
    }
    return true;
}

bool BoxRenderComponent::PostInit()
{
    m_pTransform = static_cast<Bel::TransformComponent*>(GetOwner()->GetComponent(kTransformId));
    if (m_pTransform == nullptr)
    {
        LOG_FATAL("Unable to find TransformComponent on Actor ID: ", false);
        LOG_FATAL(std::to_string(GetOwner()->GetId()));
        return false;
    }
    return true;
}

void BoxRenderComponent::Render(IGraphics* pGraphics)
{
    Rect rect = 
    { 
        static_cast<int32_t>(m_pTransform->GetPosition().m_x),
        static_cast<int32_t>(m_pTransform->GetPosition().m_y),
        m_width, 
        m_height 
    };
    Color color = { 0, 0, 255 };
    pGraphics->RenderFillRect(rect, color);
}
