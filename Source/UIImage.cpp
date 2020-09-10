#include <Core/Layers/ApplicationLayer.h>

#include "UIImage.h"
#include "ComponentIDs.h"

using namespace Bel;

/****************************************************************
 *                      TextureComponent
 ***************************************************************/
bool UIImage::Initialize(tinyxml2::XMLElement* pData)
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

    return true;
}

bool UIImage::PostInit()
{
    m_pTransform = static_cast<Bel::TransformComponent*>(GetOwner()->GetComponent(kTransformId));
    if (m_pTransform == nullptr)
    {
        LOG_FATAL("Unable to find Bel::TransformComponent on Actor");
        return false;
    }

    int posX = static_cast<int>(m_pTransform->GetPosition().m_x);
    int posY = static_cast<int>(m_pTransform->GetPosition().m_y);

    m_pTexture->SetRenderStartPosition(posX, posY);

    return true;
}

void UIImage::Update(float delta)
{
    m_pTexture->SetRenderStartPosition(static_cast<int>(m_pTransform->GetPosition().m_x), static_cast<int>(m_pTransform->GetPosition().m_y));
}

void UIImage::Render(IGraphics* pGraphics)
{
    pGraphics->DrawTexture(m_pTexture.get());
}
