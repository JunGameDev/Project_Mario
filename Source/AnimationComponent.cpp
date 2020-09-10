#include <Core/Layers/ApplicationLayer.h>

#include "AnimationComponent.h"
#include "TextureComponent.h"
#include "ComponentIDs.h"

using namespace Bel;

namespace Lua
{
    static int AnimationComponentSetAnimation(lua_State* pState)
    {
        AnimationComponent* pAnim = reinterpret_cast<AnimationComponent*>(lua_touserdata(pState, 1));
        const char* pAnimName = static_cast<const char*>(lua_tostring(pState, 2));
        pAnim->SetAnimation(pAnimName);

        lua_pop(pState, 3);
        return 0;
    }
}

bool AnimationComponent::Initialize(tinyxml2::XMLElement* pData)
{
    auto pElement = pData->FirstChildElement("InitialAnim");
    if(pElement)
        m_currentAnim = pElement->GetText();

    for (pElement = pElement->NextSiblingElement(); pElement; pElement = pElement->NextSiblingElement())
    {
        Animation anim;

        if (pElement != nullptr)
        {
            std::string animName = pElement->Attribute("name");
            const int kMaxFrame = pElement->IntAttribute("frame");
            anim.m_animationSpeed = pElement->FloatAttribute("speed");
            
            int32_t width(0);
            int32_t height(0);
            int32_t xSrcPos(0);
            int32_t ySrcPos(0);

            auto pSize = pElement->FirstChildElement("Size");
            
            if (pSize)
            {
                xSrcPos = pElement->IntAttribute("xPos");
                ySrcPos = pElement->IntAttribute("yPos");

                width  = pSize->IntAttribute("width");
                height = pSize->IntAttribute("height");

                for (int frameIndex = 0; frameIndex < kMaxFrame; ++frameIndex)
                {
                    Frame frame;
                    frame.m_sourcePosition = Bel::Rect
                    {
                        static_cast<int>(xSrcPos + (frameIndex * width)),
                        static_cast<int>(ySrcPos),
                        width, 
                        height 
                    };

                    auto pOffset = pElement->FirstChildElement("Offset");
                    if (pOffset)
                    {
                        frame.m_offset =
                        {
                            pOffset->IntAttribute("x"),
                            pOffset->IntAttribute("y")
                        };
                    }
                    else
                    {
                        frame.m_offset = { 0, 0 };
                    }

                    auto pDest = pElement->FirstChildElement("Dest");
                    if (pDest)
                    {
                        frame.m_dest =
                        {
                            pDest->IntAttribute("width"),
                            pDest->IntAttribute("height")
                        };
                    }
                    else
                    {
                        frame.m_dest = { 0, 0 };
                    }
                    anim.m_frames.emplace_back(frame);
                }
            }
            else
            {
                for (auto pChild = pElement->FirstChildElement(); pChild; pChild = pChild->NextSiblingElement())
                {
                    width   = pChild->IntAttribute("width");
                    height  = pChild->IntAttribute("height");

                    xSrcPos = pChild->IntAttribute("xPos");
                    ySrcPos = pChild->IntAttribute("yPos");

                    Frame frame;
                    frame.m_sourcePosition = Bel::Rect
                    {
                        xSrcPos,
                        ySrcPos,
                        width, 
                        height
                    };

                    frame.m_offset =
                    {
                        pChild->IntAttribute("xOffset"),
                        pChild->IntAttribute("yOffset")
                    };

                    frame.m_dest =
                    {
                        pChild->IntAttribute("destWidth"),
                        pChild->IntAttribute("destHeight")
                    };

                    anim.m_frames.emplace_back(frame);
                }
            }

            anim.m_shouldLoop = pElement->BoolAttribute("loop");
            anim.m_frameDuration = pElement->FloatAttribute("duration");

            m_animMap.emplace(animName, anim);
        }
    }

    SetAnimation(m_currentAnim);
    return true;
}

bool AnimationComponent::PostInit()
{
    m_pTexture = static_cast<TextureComponent*>(GetOwner()->GetComponent(kTextureId));
    if(!m_pTexture)
    {
        LOG_FATAL("Unable to acquire TextureComponent");
        return false;
    }

    return true;
}

void AnimationComponent::Update(float delta)
{
    Animation* pCurrentAnim = &m_animMap[m_currentAnim];
    m_frameTimer += (float)(delta * pCurrentAnim->m_animationSpeed);

    if (m_frameTimer >= pCurrentAnim->m_frameDuration)
    {
        // GoTo Next Frame!
        m_frameTimer -= pCurrentAnim->m_frameDuration;
        ++m_currentFrameIndex;

        if (pCurrentAnim->m_shouldLoop)
        {
            m_currentFrameIndex %= pCurrentAnim->m_frames.size();
        }
        else if (m_currentFrameIndex >= static_cast<int>(pCurrentAnim->m_frames.size()))
        {
            // At the end of the anim, and it's not supposed to loop
            m_currentFrameIndex = static_cast<int>(pCurrentAnim->m_frames.size()) - 1;
        }
    }

    auto currentFrame = m_animMap[m_currentAnim].m_frames[m_currentFrameIndex];
    
    Bel::Rect destRect =
    {
        m_pTexture->GetDestRect().m_x + currentFrame.m_offset.m_x,
        m_pTexture->GetDestRect().m_y + currentFrame.m_offset.m_y,
        currentFrame.m_dest.m_x,
        currentFrame.m_dest.m_y
    }; 

    m_pTexture->SetResourceRect(currentFrame.m_sourcePosition);
    m_pTexture->SetDestRect(destRect);
    //m_pTexture->SetImageSize(currentFrame.m_dest.m_x, currentFrame.m_dest.m_y);
}

void AnimationComponent::RegisterWithScript()
{
    auto& scripting = ApplicationLayer::GetInstance()->GetGameLayer()->GetScriptingManager();
    scripting.GetGlobal("g_logic");
    scripting.GetFromTable("actors");
    scripting.GetFromTable(GetOwner()->GetId());

    scripting.CreateTable();
    scripting.AddToTable("this", this);
    scripting.AddToTable("SetAnimation", Lua::AnimationComponentSetAnimation);
    scripting.AddToTable("AnimationComponent");
    scripting.PopAll();
}


void AnimationComponent::SetAnimation(std::string_view str)
{
    if(m_currentAnim != str)
    {
        if(m_animMap.find(str.data()) != m_animMap.end())
        {
            m_currentFrameIndex = 0;
            m_currentAnim = str;
            m_frameTimer = 0;
        }
    }
}
