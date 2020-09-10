#include <Core/Layers/ApplicationLayer.h>
#include "ParticleComponent.h"

using namespace Bel;

bool ParticleComponent::Initialize(tinyxml2::XMLElement* pData)
{
    auto pElement = pData->FirstChildElement("Image");
    if (pElement)
    {
        const char* pPath = pElement->Attribute("path");
        m_pTexture = ApplicationLayer::GetInstance()->GetGraphics()->LoadTextureFromCache(pPath);
        
        const Rect kRect = { pElement->IntAttribute("x"),
                                    pElement->IntAttribute("y"),
                                    pElement->IntAttribute("w"),
                                    pElement->IntAttribute("h")};

        m_pTexture->SetResourceRect(kRect.m_x, kRect.m_y, kRect.m_w, kRect.m_h);
    }

    return true;
}

void ParticleComponent::Update(float delta)
{
}

void ParticleComponent::Create(int amount)
{
    for (int index = 0; index < amount; ++index)
    {
        if (m_freeIndex >= kMaxParticles)
        {
            break;
        }

        Particle* pParticle = &m_particles[index];

        pParticle->m_pTexture = m_pTexture.get();
        pParticle->m_x = 1;
        pParticle->m_y = 1;

        float scale = m_rng.RollRandomFloatInRange(0.5f, 1.0f);
        pParticle->m_w = 1.28f * scale;
        pParticle->m_h = 1.28f * scale;

        m_rng.RollRandomDirection2D(pParticle->m_xVel, pParticle->m_yVel);
        pParticle->m_xVel *= 10;
        pParticle->m_yVel *= 10;

        pParticle->m_lifetime = pParticle->m_startingLifetime = 1;
        pParticle->m_inUse = true;
        pParticle->m_useGravity = true;
        pParticle->m_fadeAway = true;

        pParticle->m_rotVel = m_rng.RollRandomFloatInRange(-90.0f, 90.0f);

        pParticle->m_color.m_a = m_rng.RollRandomIntInRange(127, 255);
        pParticle->m_color.m_r = m_rng.RollRandomByte();
        pParticle->m_color.m_g = m_rng.RollRandomByte();
        pParticle->m_color.m_b = m_rng.RollRandomByte();
    }
}

void ParticleComponent::Render(IGraphics* pGraphics)
{
    constexpr float kPixelsPerMeter = 100;

    for (int index = 0; index < kMaxParticles; ++index)
    {
        if (m_particles[index].m_inUse)
        {
            Particle* pParticle = &m_particles[index];

            Rect dest;
            dest.m_x = (int)(pParticle->m_x * kPixelsPerMeter);
            dest.m_y = (int)(pParticle->m_y * kPixelsPerMeter);
            dest.m_w = (int)(pParticle->m_w * kPixelsPerMeter);
            dest.m_h = (int)(pParticle->m_h * kPixelsPerMeter);

            pParticle->m_pTexture->SetDestRect(dest.m_x, dest.m_y, dest.m_w, dest.m_h);

            pParticle->m_pTexture->SetTextureColor(pParticle->m_color.m_r, pParticle->m_color.m_g, pParticle->m_color.m_b);

            if (pParticle->m_fadeAway)
            {
                uint8_t startAlpha = pParticle->m_color.m_a;
                float lifetimeRatio = pParticle->m_lifetime / pParticle->m_startingLifetime;

                pParticle->m_pTexture->SetTextureAlpha(static_cast<uint8_t>(startAlpha * lifetimeRatio));
            }
            else
            {
                pParticle->m_pTexture->SetTextureAlpha(pParticle->m_color.m_a);
            }

            Point center{ dest.m_w >> 1, dest.m_h >> 1 };
            pGraphics->DrawTexture(pParticle->m_pTexture);
        }
    }
}

bool ParticleComponent::IsAnyParticleInUse()
{
    bool hellYeahThereIs = false;
    
    for (int index = 0; index < kMaxParticles; ++index)
    {
        if(m_particles[index].m_inUse)
        {
            hellYeahThereIs = true;
            break;
        }
    }

    return hellYeahThereIs;
}

void ParticleComponent::SetAllInUse(bool inUse)
{
    for (int index = 0; index < kMaxParticles; ++index)
    {
        m_particles[index].m_inUse = inUse;
    }
}

ParticleComponent::Range::Range(float min, float max)
    : m_min(min)
    , m_max(max)
{
}

float ParticleComponent::Range::Roll(SquirrelRNG& rng)
{
    return rng.RollRandomFloatInRange(m_min, m_max);
}