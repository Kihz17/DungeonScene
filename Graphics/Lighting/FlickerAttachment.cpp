#include "FlickerAttachment.h"

static float GetRandom(float low, float high)
{
	return low + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX) / (high - low));
}
FlickerAttachment::FlickerAttachment(Ref<Light> light)
	: LightAttachment(light), flickerTime(0.1f)
{

}

void FlickerAttachment::OnUpdate(float deltaTime)
{
	flickerTime += deltaTime;
	if (flickerTime >= 0.05f)
	{
		light->EditAttenuation(light->attenuation.x, GetRandom(0.133f, 0.026f), light->attenuation.z, light->attenuation.w);
		flickerTime = 0.0f;
	}
}

void FlickerAttachment::Save(YAML::Emitter & emitter) const
{

}

void FlickerAttachment::Load(const YAML::Node& node) const
{

}