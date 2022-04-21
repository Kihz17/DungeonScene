#include "TextureManager.h"

std::unordered_map<std::string, Ref<Texture>> TextureManager::loadedTextures;

Ref<DiffuseTexture> TextureManager::LoadDiffuseTexture(const std::string& path, TextureFilterType filterType, TextureWrapType wrapType, bool genMipMaps)
{
	std::unordered_map<std::string, Ref<Texture>>::iterator it = loadedTextures.find(path);
	if (it != loadedTextures.end())
	{
		return std::static_pointer_cast<DiffuseTexture>(it->second);
	}

	Ref<DiffuseTexture> texture = CreateRef<DiffuseTexture>(path, filterType, wrapType, genMipMaps);
	loadedTextures.insert(std::make_pair(path, texture));
	return texture;
}

Ref<HeightMapTexture> TextureManager::LoadHeightmapTexture(const std::string& path, TextureFilterType filterType, TextureWrapType wrapType, const glm::vec3& offset, float scale)
{
	std::unordered_map<std::string, Ref<Texture>>::iterator it = loadedTextures.find(path);
	if (it != loadedTextures.end())
	{
		return std::static_pointer_cast<HeightMapTexture>(it->second);
	}

	Ref<HeightMapTexture> texture = CreateRef<HeightMapTexture>(path, filterType, wrapType, scale);
	texture->SetOffset(offset);
	loadedTextures.insert(std::make_pair(path, texture));
	return texture;
}

Ref<DiscardTexture> TextureManager::LoadDiscardTexture(const std::string& path, TextureFilterType filterType, TextureWrapType wrapType, bool genMipMaps)
{
	std::unordered_map<std::string, Ref<Texture>>::iterator it = loadedTextures.find(path);
	if (it != loadedTextures.end())
	{
		return std::static_pointer_cast<DiscardTexture>(it->second);
	}

	Ref<DiscardTexture> texture = CreateRef<DiscardTexture>(path, filterType, wrapType, genMipMaps);
	loadedTextures.insert(std::make_pair(path, texture));
	return texture;
}

Ref<AlphaTexture> TextureManager::LoadAlphaTexture(const std::string& path, TextureFilterType filterType, TextureWrapType wrapType, bool genMipMaps)
{
	std::unordered_map<std::string, Ref<Texture>>::iterator it = loadedTextures.find(path);
	if (it != loadedTextures.end())
	{
		return std::static_pointer_cast<AlphaTexture>(it->second);
	}

	Ref<AlphaTexture> texture = CreateRef<AlphaTexture>(path, filterType, wrapType, genMipMaps);
	loadedTextures.insert(std::make_pair(path, texture));
	return texture;
}