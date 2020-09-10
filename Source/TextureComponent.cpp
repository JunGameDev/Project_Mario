#include <Core/Layers/ApplicationLayer.h>
#include "TextureComponent.h"
#include "ComponentIDs.h"
#include "Game.h"
#include "Core/Camera/Camera.h"


using namespace Bel;
#include "TextureComponent.gen"

namespace Lua
{
    static int TextureComponentSetCenter(lua_State* pState)
    {
        return 0;
    }

    static int TextureComponentSetColor(lua_State* pState)
    {
        return 0;
    }
}
/****************************************************************
 *                      TextureComponent
 ***************************************************************/
bool TextureComponent::Initialize(tinyxml2::XMLElement* pData)
{
    auto pElement = pData->FirstChildElement("Image");
    if (pElement != nullptr)
    {
        const char* pPath = pElement->Attribute("path");
        m_pTexture = ApplicationLayer::GetInstance()->GetGraphics()->LoadTextureFromCache(pPath);
    }

    pElement = pData->FirstChildElement("SourceRect");
    if (pElement != nullptr)
    {
        int x = pElement->IntAttribute("x");
        int y = pElement->IntAttribute("y");
        int w = pElement->IntAttribute("w");
        int h = pElement->IntAttribute("h");
        m_pTexture->SetResourceRect(x, y, w, h);
    }

    pElement = pData->FirstChildElement("DestRect");
    if (pElement != nullptr)
    {
        int w = pElement->IntAttribute("width");
        int h = pElement->IntAttribute("height");
        m_pTexture->SetDestRect(0, 0, w, h);
    }

    pElement = pData->FirstChildElement("Color");
    if (pElement != nullptr)
    {
        int alpha = pElement->IntAttribute("alpha");
        int r = pElement->IntAttribute("r");
        int g = pElement->IntAttribute("g");
        int b = pElement->IntAttribute("b");

        m_pTexture->SetTextureAlpha(alpha);
        m_pTexture->SetTextureColor(r, g, b);
    }

    pElement = pData->FirstChildElement("Angle");
    if (pElement != nullptr)
    {
        float degree = pElement->FloatAttribute("degree");
        m_pTexture->SetAngle(degree);
    }

    pElement = pData->FirstChildElement("Center");
    if (pElement != nullptr)
    {
        const char* pValGuard = pElement->Attribute("x");
        int xOffset = 0;
        if (pValGuard == "null")
            xOffset = ((m_pTexture->GetDestRect().m_x + (m_pTexture->GetDestRect().m_x + m_pTexture->GetDestRect().m_w)) / 2);
        else
            xOffset = pElement->IntAttribute("x");
        
        pValGuard = pElement->Attribute("y");
        int yOffset = 0;
        if (pValGuard == "null")
            yOffset = ((m_pTexture->GetDestRect().m_y + (m_pTexture->GetDestRect().m_y + m_pTexture->GetDestRect().m_h)) / 2);
        else
            yOffset = pElement->IntAttribute("y");

        SetCenter(xOffset, yOffset);
    }
    return true;
}

bool TextureComponent::PostInit()
{
    m_pTransform = static_cast<Bel::TransformComponent*>(GetOwner()->GetComponent(kTransformId));
    if (m_pTransform == nullptr)
    {
        LOG_FATAL("Unable to find TransformComponent on Actor");
        return false;
    }

    const int kPosX = static_cast<int>(m_pTransform->GetPosition().m_x);
    const int kPosY = static_cast<int>(m_pTransform->GetPosition().m_y);

    m_pTexture->SetRenderStartPosition(kPosX, kPosY);

    return true;
} 

void TextureComponent::Update(float delta)   
{
    const auto& kRenderingPoint = Camera.GetRenderingPoint();
    Rect destRect = GetDestRect();

    const int kPosX = static_cast<int>(m_pTransform->GetPosition().m_x);
    const int kPosY= static_cast<int>(m_pTransform->GetPosition().m_y);

    m_pTexture->SetRenderStartPosition(kPosX, kPosY);
    m_pTexture->SetAngle(m_pTransform->GetDegree());
    //m_pTexture->SetCenter((destRect.m_x + (destRect.m_x + destRect.m_w)) / 2, (destRect.m_y + (destRect.m_y + destRect.m_h)) / 2);
}

void TextureComponent::Render(IGraphics* pGraphics)
{
    pGraphics->DrawTexture(m_pTexture.get());
}

void TextureComponent::SetCenter(const int& x, const int& y)
{
    m_pTexture->SetCenter(x, y);
}