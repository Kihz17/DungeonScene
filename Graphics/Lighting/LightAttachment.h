#pragma once

#include "pch.h"
#include "Light.h"

#include <string>

class LightAttachment
{
public:
	LightAttachment(Ref<Light> light) : light(light) {}

	virtual ~LightAttachment() = default;

	virtual void OnUpdate(float deltaTime) = 0;
	virtual std::string GetType() const = 0;

	virtual void Save(YAML::Emitter& emitter) const = 0;
	virtual void Load(const YAML::Node& node) const = 0;
protected:
	Ref<Light> light;
};