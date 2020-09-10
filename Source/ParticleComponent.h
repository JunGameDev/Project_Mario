#pragma once
#include <Actors/Actor.h>
#include <Graphics/Graphics.h>
#include "SquirrelRNG.h"


class ParticleComponent : public Bel::IActorComponent
{
    constexpr static int kMaxParticles = 1000;
private:
    struct ParticleNode
    {
        ParticleNode* pNext;
        Bel::Particle* pData;
    };

    struct Range
    {
        float m_min, m_max;
        Range(float min, float max);

        float Roll(SquirrelRNG& rng);
    };

    int m_freeIndex;
    SquirrelRNG m_rng;

    std::shared_ptr<Bel::ITexture2D> m_pTexture;
    Bel::Particle m_particles[kMaxParticles];

public:
    ParticleComponent(Bel::Actor* pOwner, const char* pName)
        : IActorComponent(pOwner, pName)
        , m_freeIndex(0)
    {
    }

    virtual ~ParticleComponent() {}
    virtual bool Initialize(tinyxml2::XMLElement* pData) override;
    virtual void Update(float delta) override;
    virtual void Render(Bel::IGraphics* pGraphics) override;
    Bel::Particle* GetParticles() { return m_particles; }
    void DecreaseFreeIndex() { --m_freeIndex; }
    bool IsAnyParticleInUse();
    void SetAllInUse(bool inUse);
    void Create(int amount);
};

