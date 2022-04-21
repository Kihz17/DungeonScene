#pragma once

#include "DiffuseTexture.h"
#include "HeightMapTexture.h"
#include "DiscardTexture.h"
#include "AlphaTexture.h"

#include <unordered_map>

class TextureManager
{
public:
	static Ref<DiffuseTexture> LoadDiffuseTexture(const std::string& path, TextureFilterType filterType, TextureWrapType wrapType, bool genMipMaps = true);
	static Ref<HeightMapTexture> LoadHeightmapTexture(const std::string& path, TextureFilterType filterType, TextureWrapType wrapType, const glm::vec3& offset, float scale = 1.0f);
	static Ref<DiscardTexture> LoadDiscardTexture(const std::string& path, TextureFilterType filterType, TextureWrapType wrapType, bool genMipMaps = true);
	static Ref<AlphaTexture> LoadAlphaTexture(const std::string& path, TextureFilterType filterType, TextureWrapType wrapType, bool genMipMaps = true);

	static std::unordered_map<std::string, Ref<Texture>> loadedTextures;
private:

};