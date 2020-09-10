#pragma once
#include <Actor.h>
#include <Graphics.h>
#include <Physics.h>

class TransformComponent;

class ICollider2D : public Bel::IActorComponent
{
protected:
    Bel::Rect m_size;
    Bel::Point m_center;
    Bel::IGraphics* m_pGraphics;

    bool m_collide;
    TransformComponent* m_pOwnerTransform;

public:
    ICollider2D(Bel::Actor* pOwner, const char* pName)
        : Bel::IActorComponent(pOwner, pName)
        , m_size({ 0, 0, 0, 0 })
        , m_collide(false)
    {
    }

    virtual ~ICollider2D() override {};
    virtual bool Initialize(tinyxml2::XMLElement* pData) override { return true; }

    const Bel::Rect GetSize() { return m_size; }

    virtual void SetPosX(const int& x)                      = 0;
    virtual void SetPosY(const int& y)                      = 0;
    virtual void SetPosition(const int& x, const int& y)    = 0;

    virtual void SetSize(const int& w, const int& h)        = 0;
    virtual void SetWidth(const int& width)                 = 0;
    virtual void SetHeight(const int& height)               = 0;

    virtual void RenderDebug() = 0;
    
    bool IsCollide() { return m_collide; }
    void SetCollide(const bool& collide)        { m_collide = collide; }
    void SetCenter(const int& x, const int& y)  { m_center = { x, y }; }

    virtual void BeginActorTouch(ICollider2D* pOther) = 0;
    virtual void EndActorTouch(ICollider2D* pOther)   = 0;
};

class BoxCollider2D : public ICollider2D
{
public:
    BoxCollider2D(Bel::Actor* pOwner, const char* pName)
        : ICollider2D(pOwner, pName)
    {

    }

    virtual ~BoxCollider2D() override {};
    virtual bool Initialize(tinyxml2::XMLElement* pData) override;
    virtual bool PostInit() override;
    virtual void Update(float delta) override;


    virtual void SetPosition(const int& x, const int& y)    override final;
    virtual void SetSize(const int& w, const int& h)        override final;
    virtual void SetPosX(const int& x)                      override final;
    virtual void SetPosY(const int& y)                      override final;
    virtual void SetWidth(const int& width)                 override final;
    virtual void SetHeight(const int& height)               override final;

    virtual void RenderDebug() override;

    virtual void BeginActorTouch(ICollider2D* pOther)   override;
    virtual void EndActorTouch(ICollider2D* pOther)     override;
};

class RaderCollider : public BoxCollider2D
{
public:
    RaderCollider(Bel::Actor* pOwner, const char* pName)
        : BoxCollider2D(pOwner, pName)
    {

    }

    virtual ~RaderCollider() override {};
    virtual bool Initialize(tinyxml2::XMLElement* pData) override;
    virtual bool PostInit() override;
    virtual void Update(float delta) override;
    virtual void RenderDebug() override;

    virtual void BeginActorTouch(ICollider2D* pOther)   override;
    virtual void EndActorTouch(ICollider2D* pOther)     override;
};
