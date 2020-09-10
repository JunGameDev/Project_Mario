#pragma once
#include <Actors/Actor.h>
#include <Graphics/Graphics.h>
#include <Physics/Physics.h>

class BoxRenderComponent : public Bel::IActorComponent
{
private:
    Bel::Color m_color;

    int32_t m_width;
    int32_t m_height;

    Bel::TransformComponent* m_pTransform;

public:
    BoxRenderComponent(Bel::Actor* pOwner, const char* pName)
        : Bel::IActorComponent(pOwner, pName)
        , m_width(0)
        , m_height(0)
        , m_color({0, 0, 0, 0})
    {
    }
    virtual ~BoxRenderComponent() {}

    virtual bool Initialize(tinyxml2::XMLElement* pData) override;
    virtual bool PostInit() override;
    virtual void Render(Bel::IGraphics* pGraphics) override;

    int32_t GetWidth() const    { return m_width; }
    Bel::Color GetColor() const      { return m_color; }

    void SetColor(const Bel::Color&& color)      { m_color = color; }
    void SetWidth(const int32_t&& width)    { m_width = width; }
    void SetHeight(const int32_t&& height)  { m_height = height; }
};
