#pragma once
#include <Resources/Resource.h>
#include <Audio/Audio.h>
#include <mmeapi.h>

class SoundResourceExtraData : public Bel::IResourceExtraData
{
private:
protected:
    Bel::SoundType  m_soundType;
    bool            m_initialized;
    int             m_lengthMilliSecs;
    WAVEFORMATEX    m_wavFormatEx;

public:
    SoundResourceExtraData();
    virtual ~SoundResourceExtraData() override;

    virtual std::string ToString() override { return "SoundResourceExtraData"; }
    Bel::SoundType GetSoundType() { return m_soundType; }
    void SetSoundType(const Bel::SoundType& kType) { m_soundType = kType; }
    WAVEFORMATEX* const GetFormat()  { return &m_wavFormatEx; }
    void SetMilliSeconds(const int& milliSecs) { m_lengthMilliSecs = milliSecs; }
};

class WaveResourceLoader : public Bel::IResourceLoader
{
public:
    virtual bool            UseRawFile() { return false; }
    virtual bool            DiscardpRawBufferAfterLoad() { return true; }
    virtual unsigned int    GetLoadedResourceSize(char* pRawBuffer, unsigned int rawSize);
    virtual bool            LoadResource(char* pRawBuffer, unsigned int rawSize, std::shared_ptr<Bel::ResourceHandle> pHandle);
    virtual std::string     GetPattern() { return "*.wav"; }

protected:
    bool ParseWave(char* wavStream, size_t bufferLength, std::shared_ptr<Bel::ResourceHandle> pHandle);
};