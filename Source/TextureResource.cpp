#include "TextureResource.h"

using namespace Bel;

std::string TextureResourceLoader::GetPattern()
{
    return std::string();
}

bool TextureResourceLoader::UseRawFile()
{
    return false;
}

unsigned int TextureResourceLoader::GetLoadedResourceSize(char* pRawBuffer, unsigned int rawSize)
{
    return 0;
}

bool TextureResourceLoader::LoadResource(char* pRawBuffer, unsigned int rawSize, std::shared_ptr<ResourceHandle> pHandle)
{
    return false;
}
