#include "Collider.h"
#include "ComponentIDs.h"
#include "ApplicationLayer.h"
#include "TextureComponent.h"
#include "TransformComponent.h"
#include "Game.h"

using namespace Bel;

/****************************************************************
 *                      BoxCollider2D
 ***************************************************************/
bool BoxCollider2D::Initialize(tinyxml2::XMLElement* pData)
{
    m_pGraphics = ApplicationLayer::GetInstance()->GetGraphics();

    auto pElement = pData->FirstChildElement("Size");
    if (pElement != nullptr)
    {
        int width = pElement->IntAttribute("width");
        int height = pElement->IntAttribute("height");

        this->SetSize(width, height);
    }

    pElement = pData->FirstChildElement("Center");
    if (pElement != nullptr)
    {
        int centerX = pElement->IntAttribute("x");
        int centerY = pElement->IntAttribute("y");

        this->SetCenter(centerX, centerY);
    }
    else
    {
        this->SetCenter(0, 0);
    }

    return true;
}

bool BoxCollider2D::PostInit()
{
    m_pOwnerTransform = static_cast<TransformComponent*>(GetOwner()->GetComponent(kTransformId));
    if (m_pOwnerTransform == nullptr)
    {
        LOG_FATAL("Unable to find TransformComponent on Actor");
        return false;
    }

    this->SetPosition((m_center.m_x - (m_size.m_w >> 1)), (m_center.m_y - (m_size.m_h >> 1)));

    return true;
}

void BoxCollider2D::Update(float delta)
{
    const int newPosX = static_cast<int>(m_pOwnerTransform->GetX() - (m_size.m_w >> 1) - m_center.m_x);
    const int newPosY = static_cast<int>(m_pOwnerTransform->GetY() - (m_size.m_h >> 1) - m_center.m_y);
    this->SetPosition(newPosX, newPosY);
}

void BoxCollider2D::SetPosX(const int& x)
{
    m_size.m_x = x;
}

void BoxCollider2D::SetPosY(const int& y)
{
    m_size.m_y = y;
}

void BoxCollider2D::SetWidth(const int& width)
{
    m_size.m_w = width;
}

void BoxCollider2D::SetHeight(const int& height)
{
    m_size.m_h = height;
}

void BoxCollider2D::SetPosition(const int& x, const int& y)
{
    SetPosX(x);
    SetPosY(y);
}

void BoxCollider2D::SetSize(const int& w, const int& h)
{
    SetWidth(w);
    SetHeight(h);
}

void BoxCollider2D::RenderDebug()
{
    Point camera = Camera.GetRenderingPoint();

    Rect rect = GetSize();
    rect.m_x = rect.m_x - camera.m_x;
    rect.m_y = rect.m_y - camera.m_y;
    Color color{ 0, 255, 0, 255 };
    m_pGraphics->RenderRect(rect, color);
}

void BoxCollider2D::BeginActorTouch(ICollider2D* pOther)
{
}

void BoxCollider2D::EndActorTouch(ICollider2D* pOther)
{
}

/****************************************************************
 *                      RaderCollider
 ***************************************************************/
bool RaderCollider::Initialize(tinyxml2::XMLElement* pData)
{
    m_pGraphics = ApplicationLayer::GetInstance()->GetGraphics();

    auto pElement = pData->FirstChildElement("Size");
    if (pElement != nullptr)
    {
        int width = pElement->IntAttribute("width");
        int height = pElement->IntAttribute("height");

        this->SetSize(width, height);
    }

    pElement = pData->FirstChildElement("Center");
    if (pElement != nullptr)
    {
        int centerX = pElement->IntAttribute("x");
        int centerY = pElement->IntAttribute("y");

        this->SetCenter(centerX, centerY);
    }
    else
    {
        this->SetCenter(0, 0);
    }

    return true;
}

bool RaderCollider::PostInit()
{
    m_pOwnerTransform = static_cast<TransformComponent*>(GetOwner()->GetComponent(kTransformId));
    if (m_pOwnerTransform == nullptr)
    {
        LOG_FATAL("Unable to find TransformComponent on Actor");
        return false;
    }

    this->SetPosition((m_center.m_x - (m_size.m_w >> 1)), (m_center.m_y - (m_size.m_h >> 2)));

    return true;
}

void RaderCollider::Update(float delta)
{
    const int newPosX = static_cast<int>(m_pOwnerTransform->GetX() - (m_size.m_w >> 1) - m_center.m_x);
    const int newPosY = static_cast<int>(m_pOwnerTransform->GetY() - (m_size.m_h >> 1) - m_center.m_y);
    this->SetPosition(newPosX, newPosY);
}

void RaderCollider::RenderDebug()
{
    Point camera = Camera.GetRenderingPoint();

    Bel::Rect rect = GetSize();
    rect.m_x = rect.m_x - camera.m_x;
    rect.m_y = rect.m_y - camera.m_y;
    Bel::Color color{ 255, 0, 0, 255 };
    m_pGraphics->RenderRect(rect, color);
}

void RaderCollider::BeginActorTouch(ICollider2D* pOther)
{
}

void RaderCollider::EndActorTouch(ICollider2D* pOther)
{
}
