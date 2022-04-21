#pragma once

#include "LightAttachment.h"
#include "UUID.h"

class SceneLight
{
public:
	SceneLight(Ref<Light> light) : light(light) {}

	virtual void Save(YAML::Emitter& emitter) const;
	static Ref<SceneLight> StaticLoad(const YAML::Node& node);

	UUID uuid;
	Ref<Light> light;
	std::vector<Ref<LightAttachment>> attachements;
};