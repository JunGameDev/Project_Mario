#pragma once

#include <Actors/Actor.h>
#include <Graphics/Graphics.h>
#include <Physics/Physics.h>

class Bel::TransformComponent;

class UIImage : public Bel::IActorComponent
{
private:
    Bel::TransformComponent* m_pTransform;
    std::shared_ptr<Bel::ITexture2D> m_pTexture;

public:
    UIImage(Bel::Actor* pOwner, const char* pName)
        : Bel::IActorComponent(pOwner, pName)
        , m_pTransform(nullptr)
    {
    }

    virtual ~UIImage() override {}

    virtual bool Initialize(tinyxml2::XMLElement* pData) override;
    virtual bool PostInit() override;
    virtual void Update(float delta) override;
    virtual void Render(Bel::IGraphics* pGraphics) override;

    Bel::Rect GetDestRect() { return m_pTexture->GetDestRect(); }
    void SetColor(int r, int g, int b) { m_pTexture->SetTextureColor(r, g, b); }
    void SetAlpha(int a) { m_pTexture->SetTextureAlpha(a); }
    void SetResourceRect(Bel::Rect& rect) { m_pTexture->SetResourceRect(rect.m_x, rect.m_y, rect.m_w, rect.m_h); }
    void SetDestRect(Bel::Rect& rect) { m_pTexture->SetDestRect(rect.m_x, rect.m_y, rect.m_w, rect.m_h); }

    void SetRenderFlip(Bel::RenderFlip flip) { m_pTexture->SetRenderFlip(flip); }
    Bel::RenderFlip& GetRenderFlip() { return m_pTexture->GetRenderFlip(); }

};

