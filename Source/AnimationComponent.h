#pragma once
#include <Actors/Actor.h>
#include <Graphics/Graphics.h>
#include <string_view>

class TextureComponent;

class AnimationComponent : public Bel::IActorComponent
{
public:
    struct Frame
    {
        Bel::Rect m_sourcePosition;
        Bel::Point m_offset;
        Bel::Point m_dest;
    };

    struct Animation
    {
        std::vector<Frame> m_frames;
        float m_animationSpeed;
        float m_frameDuration;
        bool m_shouldLoop;
    };

private:
    TextureComponent* m_pTexture;
    std::unordered_map<std::string, Animation> m_animMap;
    
    std::string m_currentAnim;
    int         m_currentFrameIndex;
    float       m_frameTimer;

public:
    AnimationComponent(Bel::Actor* pOwner, const char* pName)
        : IActorComponent(pOwner, pName)
        , m_pTexture(nullptr)
        , m_currentFrameIndex(0)
        , m_frameTimer(0)
    {
    }

    virtual ~AnimationComponent() override {}
    virtual bool Initialize(tinyxml2::XMLElement* pData) override;
    virtual bool PostInit() override;
    virtual void Update(float delta) override;
    virtual void Render(Bel::IGraphics* pGraphics) override {}
    LUA_REGISTER();

    void SetAnimation(std::string_view str);
};
