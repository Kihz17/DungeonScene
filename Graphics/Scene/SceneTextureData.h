#pragma once

#include "Texture.h"


class SceneTextureData
{
public:
	SceneTextureData(Ref<Texture> texture, float ratio = 1.0f, float texCoordScale = 1.0);

	virtual void Save(YAML::Emitter& emitter) const;

	static Ref<SceneTextureData> StaticLoad(YAML::Node& node);

	Ref<Texture> texture;
	float ratio;
	float texCoordScale;
};