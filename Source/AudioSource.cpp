#include "AudioSource.h"
#include <Core/Layers/ApplicationLayer.h>

using namespace Bel;

namespace Lua
{
    static int AudioSourcePlayerClip(lua_State* pState)
    {
        AudioSource* pSource = reinterpret_cast<AudioSource*>(lua_touserdata(pState, 1));
        const char* pClipName = static_cast<const char*>(lua_tostring(pState, 2));
        pSource->PlayClip(pClipName);
        LOG(pClipName);
        lua_pop(pState, 2);

        return 0;
    }
}

/****************************************************************
 *                      AudioSource
 ***************************************************************/
bool AudioSource::Initialize(tinyxml2::XMLElement* pData)
{
    for (auto pElement = pData->FirstChildElement(); pElement; pElement = pElement->NextSiblingElement())
    {
        if (pElement != nullptr)
        {
            std::string name = pElement->Attribute("name");

            AudioClip clip;
            clip.m_path = pElement->Attribute("path");
            clip.m_loop = pElement->BoolAttribute("loop");
            clip.m_playOnAwake = pElement->BoolAttribute("playOnAwake");
            clip.m_volume = pElement->IntAttribute("volume");
            clip.m_mute = pElement->BoolAttribute("mute");
            m_clips[name] = clip;
        }
    }

    return true;
}

bool AudioSource::PostInit()
{
    m_pAudio = ApplicationLayer::GetInstance()->GetAudio();
    if (m_pAudio == nullptr)
    {
        LOG_FATAL("Unable to get Audio");
        return false;
    }

    for (auto& clip : m_clips)
    {
        if (clip.second.m_playOnAwake)
        {
            PlayClip(clip.second.m_path.data());
        }
    }
    return true;
}

void AudioSource::PlayClip(const char* pName)
{
    AudioClip& clip = m_clips[pName];
    if (clip.m_path.empty())
    {
        LOG_FATAL("Incorrect Path");
        return;
    }
    m_pAudio->PlaySoundEffect(clip.m_path.c_str(), clip.m_volume);
}

void AudioSource::RegisterWithScript()
{
    auto& scripting = ApplicationLayer::GetInstance()->GetGameLayer()->GetScriptingManager();
    scripting.GetGlobal("g_logic");
    scripting.GetFromTable("actors");
    scripting.GetFromTable(GetOwner()->GetId());

    scripting.CreateTable();
    scripting.AddToTable("this", this);
    scripting.AddToTable("PlayClip", Lua::AudioSourcePlayerClip);
    scripting.AddToTable("AudioSource");

    scripting.PopAll();
}
