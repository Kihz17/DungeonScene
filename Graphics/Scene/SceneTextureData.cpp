#include "SceneTextureData.h"
#include "TextureManager.h"
#include "YAMLOverloads.h"

SceneTextureData::SceneTextureData(Ref<Texture> texture, float ratio, float texCoordScale)
	: texture(texture), ratio(ratio), texCoordScale(texCoordScale)
{

}

void SceneTextureData::Save(YAML::Emitter& emitter) const
{
	TextureType textureType = this->texture->GetType();
	emitter << YAML::BeginMap;
	emitter << YAML::Key << "TextureType" << YAML::Value << TextureTypeToString(textureType);
	emitter << YAML::Key << "Path" << YAML::Value << SerializeUtils::SavePath(this->texture->GetPath());
	emitter << YAML::Key << "Ratio" << YAML::Value << this->ratio;
	emitter << YAML::Key << "TexCoordScale" << YAML::Value << this->texCoordScale;
	emitter << YAML::Key << "FilterType" << YAML::Value << FilterTypeToString(this->texture->GetFilterType());
	emitter << YAML::Key << "WrapType" << YAML::Value << WrapTypeToString(this->texture->GetWrapType());
	if (textureType == TextureType::Diffuse)
	{
		Ref<DiffuseTexture> diffuseTexture = std::static_pointer_cast<DiffuseTexture>(this->texture);
		emitter << YAML::Key << "GenMipMaps" << YAML::Value << diffuseTexture->IsGenMipMaps();
	}
	else if (textureType == TextureType::Heightmap)
	{
		Ref<HeightMapTexture> heightMapTexture = std::static_pointer_cast<HeightMapTexture>(this->texture);
		emitter << YAML::Key << "Scale" << YAML::Value << heightMapTexture->GetScale();
		emitter << YAML::Key << "Offset" << YAML::Value << heightMapTexture->GetOffset();
	}
	else if (textureType == TextureType::Discard)
	{
		Ref<DiscardTexture> discardTexture = std::static_pointer_cast<DiscardTexture>(this->texture);
		emitter << YAML::Key << "GenMipMaps" << YAML::Value << discardTexture->IsGenMipMaps();
	}
	else if (textureType == TextureType::Alpha)
	{
		Ref<AlphaTexture> alphaTexture = std::static_pointer_cast<AlphaTexture>(this->texture);
		emitter << YAML::Key << "GenMipMaps" << YAML::Value << alphaTexture->IsGenMipMaps();
	}
	emitter << YAML::EndMap;
}

Ref<SceneTextureData> SceneTextureData::StaticLoad(YAML::Node& node)
{
	TextureType textureType = StringToTextureType(node["TextureType"].as<std::string>());
	if (textureType == TextureType::None)
	{
		std::cout << "Invalid texture type when loading!" << std::endl;
		return NULL;
	}

	std::string path = SerializeUtils::LoadPath(node["Path"].as<std::string>());
	float ratio = node["Ratio"].as<float>();
	float scale = node["TexCoordScale"].as<float>();
	TextureFilterType filterType = StringToFilterType(node["FilterType"].as<std::string>());
	TextureWrapType wrapType = StringToWrapType(node["WrapType"].as<std::string>());

	if (textureType == TextureType::Diffuse)
	{
		bool genMipMaps = node["GenMipMaps"].as<bool>();
		Ref<DiffuseTexture> texture = TextureManager::LoadDiffuseTexture(path, filterType, wrapType, genMipMaps);
		return CreateRef<SceneTextureData>(texture, ratio, scale);
	}
	else if (textureType == TextureType::Heightmap)
	{
		float scale = node["Scale"].as<float>();
		glm::vec3 offset = node["Offset"].as<glm::vec3>();
		Ref<HeightMapTexture> texture = TextureManager::LoadHeightmapTexture(path, filterType, wrapType, offset, scale);
		return CreateRef<SceneTextureData>(texture, ratio, scale);
	}
	else if (textureType == TextureType::Discard)
	{
		bool genMipMaps = node["GenMipMaps"].as<bool>();
		Ref<DiscardTexture> texture = TextureManager::LoadDiscardTexture(path, filterType, wrapType, genMipMaps);
		return CreateRef<SceneTextureData>(texture, ratio, scale);
	}
	else if (textureType == TextureType::Alpha)
	{
		bool genMipMaps = node["GenMipMaps"].as<bool>();
		Ref<AlphaTexture> texture = TextureManager::LoadAlphaTexture(path, filterType, wrapType, genMipMaps);
		return CreateRef<SceneTextureData>(texture, ratio, scale);
	}

	std::cout << "Texture type does not have serialzier!";
	return NULL;
}