#pragma once

#include "pch.h"
#include "UUID.h"
#include "Mesh.h"
#include "Serializable.h"
#include "SceneTextureData.h"

#include <glm/glm.hpp>

class SceneMeshData 
{
public:
	SceneMeshData(Ref<Mesh> mesh);
	virtual ~SceneMeshData();

	virtual void Save(YAML::Emitter& emitter) const;

	virtual void AddTexture(Ref<Texture> texture, float ratio = 1.0f, float scale = 1.0f);
	virtual void AddTexture(Ref<SceneTextureData> textureData);

	static Ref<SceneMeshData> StaticLoad(const YAML::Node& node);

	UUID uuid;
	Ref<Mesh> mesh;
	glm::vec3 position;
	glm::vec3 orientation;
	glm::vec3 scale;

	float alphaTransparency;
	bool hasAlphaTransparentTexture;

	std::vector<Ref<SceneTextureData>> textures;
};