#include "SceneMeshData.h"
#include "MeshManager.h"
#include "YAMLOverloads.h"

SceneMeshData::SceneMeshData(Ref<Mesh> mesh)
	: mesh(mesh), 
	position(0.0f), 
	orientation(0.0f, 0.0f, 0.0f), 
	scale(1.0f, 1.0f, 1.0f), 
	alphaTransparency(1.0f),
	hasAlphaTransparentTexture(false)
{

}

SceneMeshData::~SceneMeshData()
{
	
}

void SceneMeshData::Save(YAML::Emitter& emitter) const
{
	emitter << YAML::BeginMap;

	emitter << YAML::Key << "UUID" << YAML::Value << this->uuid;
	emitter << YAML::Key << "Path" << YAML::Value << SerializeUtils::SavePath(this->mesh->GetPath());
	emitter << YAML::Key << "Position" << YAML::Value << this->position;
	emitter << YAML::Key << "Orientation" << YAML::Value << this->orientation;
	emitter << YAML::Key << "Scale" << YAML::Value << this->scale;
	emitter << YAML::Key << "AlphaTransparency" << YAML::Value << this->alphaTransparency;

	emitter << YAML::Key << "Textures" << YAML::Value << YAML::BeginSeq;
	for (const Ref<SceneTextureData>& textureData : this->textures)
	{
		textureData->Save(emitter);
	}
	emitter << YAML::EndSeq;

	emitter << YAML::EndMap;
}

Ref<SceneMeshData> SceneMeshData::StaticLoad(const YAML::Node& node)
{
	std::string path = SerializeUtils::LoadPath(node["Path"].as<std::string>());
	Ref<Mesh> mesh = MeshManager::LoadMesh(path);

	Ref<SceneMeshData> meshData = CreateRef<SceneMeshData>(mesh);

	// Load textures
	const YAML::Node& textureNode = node["Textures"];
	if (textureNode)
	{
		YAML::const_iterator it;
		for (it = textureNode.begin(); it != textureNode.end(); it++)
		{
			YAML::Node childNode = (*it);
			Ref<SceneTextureData> textureData = SceneTextureData::StaticLoad(childNode);
			meshData->AddTexture(textureData); 
		}
	}

	meshData->uuid = node["UUID"].as<uint64_t>();
	meshData->position = node["Position"].as<glm::vec3>();
	meshData->orientation = node["Orientation"].as<glm::vec3>();
	meshData->scale = node["Scale"].as<glm::vec3>();
	meshData->alphaTransparency = node["AlphaTransparency"].as<float>();
	return meshData;
}

void SceneMeshData::AddTexture(Ref<Texture> texture, float ratio, float scale)
{
	Ref<SceneTextureData> textureData = CreateRef<SceneTextureData>(texture, ratio, scale);

	if (texture->GetType() == TextureType::Heightmap)
	{
		if (!this->textures.empty() && this->textures[0]->texture->GetType() == TextureType::Heightmap)
		{
			std::cout << "WARNING: Tried to add multiple heightmap textures to one mesh!" << std::endl;
			return;
		}

		this->textures.insert(this->textures.begin(), textureData); // Make sure heightmap is at the front
		return;
	}
	else if (texture->GetType() == TextureType::Alpha)
	{
		this->hasAlphaTransparentTexture = true;
	}

	this->textures.push_back(textureData);
}

void SceneMeshData::AddTexture(Ref<SceneTextureData> textureData)
{
	if (textureData->texture->GetType() == TextureType::Heightmap)
	{
		if (!this->textures.empty() && this->textures[0]->texture->GetType() == TextureType::Heightmap)
		{
			std::cout << "WARNING: Tried to add multiple heightmap textures to one mesh!" << std::endl;
			return;
		}

		this->textures.insert(this->textures.begin(), textureData); // Make sure heightmap is at the front
		return;
	}
	else if (textureData->texture->GetType() == TextureType::Alpha)
	{
		this->hasAlphaTransparentTexture = true;
	}

	this->textures.push_back(textureData);
}