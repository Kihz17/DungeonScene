#pragma once

#include "LightAttachment.h"

class FlickerAttachment : public LightAttachment
{
public:
	FlickerAttachment(Ref<Light> light);
	virtual ~FlickerAttachment() = default;

	virtual void OnUpdate(float deltaTime) override;
	inline virtual std::string GetType() const override { return "Flicker"; }

	virtual void Save(YAML::Emitter& emitter) const override;
	virtual void Load(const YAML::Node& node) const override;

private:
	float flickerTime;
};