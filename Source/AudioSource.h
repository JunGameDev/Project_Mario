#pragma once
#include <Actors/Actor.h>
#include <Audio/Audio.h>

class AudioSource : public Bel::IActorComponent
{
public:
    using AudioName = std::string;

private:
    std::unordered_map<AudioName, Bel::AudioClip> m_clips;
    Bel::IAudio* m_pAudio;

public:
    AudioSource(Bel::Actor* pOwner, const char* pName)
        : Bel::IActorComponent(pOwner, pName)
        , m_pAudio(nullptr)
    {
    }

    virtual ~AudioSource() override {}

    virtual bool Initialize(tinyxml2::XMLElement* pData) override;
    virtual bool PostInit() override;
    LUA_REGISTER();

    void PlayClip(const char* pName);
};
