#pragma once
#include <Resources/Resource.h>

class TextureResourceLoader : public Bel::IResourceLoader
{
    // Inherited via IResourceLoader
    virtual std::string GetPattern() override;
    virtual bool UseRawFile() override;
    virtual unsigned int GetLoadedResourceSize(char* pRawBuffer, unsigned int rawSize) override;
    virtual bool LoadResource(char* pRawBuffer, unsigned int rawSize, std::shared_ptr<Bel::ResourceHandle> pHandle) override;
};

