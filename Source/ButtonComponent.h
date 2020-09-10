#pragma once
#include <Actors/Actor.h>
#include <Graphics/Graphics.h>
#include <Input/Input.h>
#include <functional>
#include <Physics/Physics.h>

class UIImage;

class ButtonComponent : public Bel::IActorComponent
{
private:
    using OnClickEnter = std::function<void()>;
    using OnClickExit = std::function<void()>;
    using OnMouseEnter = std::function<void()>;
    using OnMouseExit = std::function<void()>;

    Bel::Rect m_destRect;
    //Whale::Rect m_srcRect;
    //Whale::Color m_color;

    Bel::IMouse* m_pMouse;

    OnClickEnter	m_handleClickEnter;
    OnClickExit     m_handleClickExit;
    OnMouseEnter    m_handleMouseEnter;
    OnMouseExit     m_handleMouseExit;
    
    bool m_previousMouseOnBtn;
    Bel::TransformComponent* m_pTransform;
    UIImage* m_pImage;

public:
    ButtonComponent(Bel::Actor* pOwner, const char* pName)
        : Bel::IActorComponent(pOwner, pName)
        , m_previousMouseOnBtn(false)
        , m_pTransform(nullptr)
        , m_pImage(nullptr)
    {
    }

    virtual ~ButtonComponent() override {}
    virtual bool Initialize(tinyxml2::XMLElement* pData) override;
    virtual bool PostInit() override;
    virtual void Update(float delta) override;

    const bool CheckMousePosition(int32_t x, int32_t y);

    void SetDestRect(Bel::Rect rect) { m_destRect = rect; }

    LUA_REGISTER();

    //LUA_LAMBDAGLUE(ClickEnter, handleClickEnter);
    //LUA_LAMBDAGLUE(ClickExit, handleClickExit);
    //LUA_LAMBDAGLUE(MouseEnter, handleMouseEnter);
    //LUA_LAMBDAGLUE(MouseExit, handleMouseExit);

    void SetClickEnter(std::function<void()> func) { m_handleClickEnter = std::move(func); }
    void SetClickExit(std::function<void()>  func) { m_handleClickExit  = std::move(func); }
    void SetMouseEnter(std::function<void()> func) { m_handleMouseEnter = std::move(func); }
    void SetMouseExit(std::function<void()>  func) { m_handleMouseExit  = std::move(func); }
};
