#include <Core/Layers/ApplicationLayer.h>

#include "ButtonComponent.h"
#include "ComponentIDs.h"
#include "UIImage.h"

using namespace Bel;
#include "ButtonComponent.gen"

/****************************************************************
 *                      ButtonComponent
 ***************************************************************/
bool ButtonComponent::Initialize(tinyxml2::XMLElement* pData)
{
    return true;
}

bool ButtonComponent::PostInit()
{
    m_pMouse = ApplicationLayer::GetInstance()->GetMouseInput();
    m_pTransform = static_cast<Bel::TransformComponent*>(GetOwner()->GetComponent(kTransformId));
    if (!m_pTransform)
    {
        LOG_FATAL("Unable to access owner's transform component");
        return false;
    }

    m_pImage = static_cast<UIImage*>(GetOwner()->GetComponent(kUIImageId));
    if (!m_pImage)
    {
        LOG_FATAL("Unable to access owner's texture component");
        return false;
    }

    this->SetDestRect(m_pImage->GetDestRect());

    return true;
}

void ButtonComponent::Update(float delta)
{
    m_destRect.m_x = static_cast<int32_t>(m_pTransform->GetPosition().m_x);
    m_destRect.m_y = static_cast<int32_t>(m_pTransform->GetPosition().m_y);

    Point mousePosition = { m_pMouse->GetMouseX(), m_pMouse->GetMouseY() };

    bool isMouseOnButton = CheckMousePosition(mousePosition.m_x, mousePosition.m_y);

    if (isMouseOnButton)
    {
        // Mouse entered the boundary of this button.
        if (!m_previousMouseOnBtn)
        {
            m_previousMouseOnBtn = true;

            if (m_handleMouseEnter)
            {
                m_handleMouseEnter();
                m_pImage->SetColor(128, 0, 0);
            }
        }

        if (m_pMouse->IsButtonPressed(IMouse::Button::kBtnLeft))
        {
            if (m_handleClickEnter)
                m_handleClickEnter();
            else
                LOG_FATAL("Event isn't set");
        }
    }

    if (!isMouseOnButton)
    {
        // Mouse exits the boundary of this button.
        if (m_previousMouseOnBtn)
        {
            m_previousMouseOnBtn = false;

            if (m_handleMouseExit)
            {
                m_handleMouseExit();
                m_pImage->SetColor(255, 255, 255);
            }
        }
    }
}

const bool ButtonComponent::CheckMousePosition(int32_t x, int32_t y)
{
    Point point{ x, y };
    return PointInRect(point, m_destRect);
}
