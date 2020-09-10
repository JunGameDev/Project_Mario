#include <Core/Layers/ApplicationLayer.h>
#include <memory>
#include <Windows.h>
#include <vfw.h>
#include "SoundResource.h"


using namespace Bel;

//------------------------------------------------------------------------------------------------------------
// Sound Resource Extra Data
//------------------------------------------------------------------------------------------------------------
SoundResourceExtraData::SoundResourceExtraData()
    : m_soundType(Bel::SoundType::kUnknown)
    , m_initialized(false)
    , m_lengthMilliSecs(0)
{
}

SoundResourceExtraData::~SoundResourceExtraData()
{
}

//------------------------------------------------------------------------------------------------------------
// Wave Resource Loader
//------------------------------------------------------------------------------------------------------------
unsigned int WaveResourceLoader::GetLoadedResourceSize(char* pRawBuffer, unsigned int rawSize)
{
    DWORD       file = 0;
    DWORD       fileEnd = 0;

    DWORD       length = 0;
    DWORD       type = 0;

    DWORD pos = 0;

    // mmioFOURCC -- converts four chars into a 4 byte integer code.
    // The first 4 bytes of a valid .wav file is 'R','I','F','F'

    type = *((DWORD*)(pRawBuffer + pos));
    pos += sizeof(DWORD);
    if (type != mmioFOURCC('R', 'I', 'F', 'F'))
        return false;

    length = *((DWORD*)(pRawBuffer + pos));
    pos += sizeof(DWORD);
    type = *((DWORD*)(pRawBuffer + pos));       
    pos += sizeof(DWORD);

    // 'W','A','V','E' for a legal .wav file
    if (type != mmioFOURCC('W', 'A', 'V', 'E'))
        return false;       //not a WAV

    // Find the end of the file
    fileEnd = length - 4;

    bool copiedBuffer = false;

    // Load the .wav format and the .wav data
    // Note that these blocks can be in either order.
    while (file < fileEnd)
    {
        type = *((DWORD*)(pRawBuffer + pos));
        pos += sizeof(DWORD);
        file += sizeof(DWORD);

        length = *((DWORD*)(pRawBuffer + pos));
        pos += sizeof(DWORD);
        file += sizeof(DWORD);

        switch (type)
        {
        case mmioFOURCC('f', 'a', 'c', 't'):
        {
            LOG_FATAL("This wav file is compressed.  We don't handle compressed wav at this time");
            break;
        }

        case mmioFOURCC('f', 'm', 't', ' '):
        {
            pos += length;
            break;
        }

        case mmioFOURCC('d', 'a', 't', 'a'):
        {
            return length;
        }
        }

        file += length;

        // Increment the pointer past the block we just read,
        // and make sure the pointer is word aliged.
        if (length & 1)
        {
            ++pos;
            ++file;
        }
    }

    // If we get to here, the .wav file didn't contain all the right pieces.
    return false;
}


bool WaveResourceLoader::LoadResource(char* pRawBuffer, unsigned int rawSize, std::shared_ptr<Bel::ResourceHandle> pHandle)
{
    std::shared_ptr<SoundResourceExtraData> pExtra = std::make_shared<SoundResourceExtraData>();
    pExtra->SetSoundType(SoundType::kType_Wav);
    pHandle->SetExtra(pExtra);
    if (!ParseWave(pRawBuffer, rawSize, pHandle))
    {
        return false;
    }

    return true;
}

bool WaveResourceLoader::ParseWave(char* pWavStream, size_t bufferLength, std::shared_ptr<Bel::ResourceHandle> pHandle)
{
    std::shared_ptr<SoundResourceExtraData> pExtra = std::static_pointer_cast<SoundResourceExtraData>(pHandle->GetExtra());

    
    DWORD file = 0;
    DWORD fileEnd = 0;
    DWORD length = 0;
    DWORD type = 0;
    DWORD pos = 0;

    type = *((DWORD*)(pWavStream + pos)); 
    pos += sizeof(DWORD); 
    
    if (type != mmioFOURCC('R', 'I', 'F', 'F')) 
        return false;
    
    length = *((DWORD*)(pWavStream + pos)); 
    pos += sizeof(DWORD); 

    type = *((DWORD*)(pWavStream + pos)); 
    pos += sizeof(DWORD);
    
    // 'W','A','V','E' for a legal .wav file 
    if(type != mmioFOURCC('W', 'A', 'V', 'E')) 
        return false; //not a WAV

    // Find the end of the file 
    fileEnd = length - 4;

    memset(pExtra->GetFormat(), 0, sizeof(WAVEFORMATEX));

    bool copiedBuffer = false;

    // Load the .wav format and the .wav data
    // Note that these blocks can be in either order.
    while (file < fileEnd)
    {
        type = *((DWORD*)(pWavStream + pos));
        pos += sizeof(DWORD);
        file += sizeof(DWORD);

        length = *((DWORD*)(pWavStream + pos));
        pos += sizeof(DWORD);
        file += sizeof(DWORD);
        
        switch (type)
        {
        case mmioFOURCC('f', 'a', 'c', 't'): 
        {
            break;
        }
        case mmioFOURCC('f', 'm', 't', ' '): 
        {
            memcpy(pExtra->GetFormat(), pWavStream + pos, length);
            pos += length;
            pExtra->GetFormat()->cbSize = static_cast<WORD>(length); 
            break;
        }
        case mmioFOURCC('d', 'a', 't', 'a'): 
        {
            copiedBuffer = true; 
            if (length != pHandle->GetSize()) 
            {
                return 0;
            }
            memcpy(&pHandle->GetData(), pWavStream + pos, length); 
            pos += length; 
            break;
        }
        }

        file += length;

        // If both blocks have been seen, we can return true.
        if (copiedBuffer)
        {
            pExtra->SetMilliSeconds(static_cast<int>((pHandle->GetSize() * 1000) / pExtra->GetFormat()->nAvgBytesPerSec));
            return true;
        }

        // Increment the pointer past the block we just read,
        // and make sure the pointer is Word aliged.
        if (length & 1)
        {
            ++pos;
            ++file;
        }
    }

    // If we get to here, the .wav file didnt' contain all the right pieces.
    return false;
}
