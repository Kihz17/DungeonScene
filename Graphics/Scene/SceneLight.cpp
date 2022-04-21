#include "SceneLight.h"
#include "YAMLOverloads.h"
#include "FlickerAttachment.h"
void SceneLight::Save(YAML::Emitter& emitter) const
{
	emitter << YAML::BeginMap;
	emitter << YAML::Key << "UUID" << YAML::Value << uuid;
	emitter << YAML::Key << "Index" << YAML::Value << light->index;
	emitter << YAML::Key << "Position" << YAML::Value << light->position;
	emitter << YAML::Key << "Diffuse" << YAML::Value << light->diffuse;
	emitter << YAML::Key << "Specular" << YAML::Value << light->specular;
	emitter << YAML::Key << "Attenuation" << YAML::Value << light->attenuation;
	emitter << YAML::Key << "Direction" << YAML::Value << light->direction;
	emitter << YAML::Key << "LightType" << YAML::Value << LightTypeToString(light->lightType);
	emitter << YAML::Key << "OuterAngle" << YAML::Value << light->outerAngle;
	emitter << YAML::Key << "InnerAngle" << YAML::Value << light->innerAngle;
	emitter << YAML::Key << "State" << YAML::Value << light->state;

	emitter << YAML::Key << "Attachments" << YAML::BeginSeq;
	for (Ref<LightAttachment> attch : attachements)
	{
		emitter << YAML::Key << attch->GetType() << YAML::Value;
		attch->Save(emitter);
	}
	emitter << YAML::EndSeq;

	emitter << YAML::EndMap;
}

Ref<SceneLight> SceneLight::StaticLoad(const YAML::Node& node)
{
	UUID uuid = node["UUID"].as<uint64_t>();
	unsigned int index = node["Index"].as<unsigned int>();
	glm::vec4 position = node["Position"].as<glm::vec4>();
	glm::vec4 diffuse = node["Diffuse"].as<glm::vec4>();
	glm::vec4 specular = node["Specular"].as<glm::vec4>();
	glm::vec4 attenuation = node["Attenuation"].as<glm::vec4>();
	glm::vec4 direction = node["Direction"].as<glm::vec4>();
	Light::LightType lightType = StringToLightType(node["LightType"].as<std::string>());
	float outerAngle = node["OuterAngle"].as<float>();
	float innerAngle = node["InnerAngle"].as<float>();
	bool state = node["State"].as<bool>();

	Ref<Light> light = CreateRef<Light>(index);
	light->position = position;
	light->diffuse = diffuse;
	light->specular = specular;
	light->attenuation = attenuation;
	light->direction = direction;
	light->lightType = lightType;
	light->outerAngle = outerAngle;
	light->innerAngle = innerAngle;
	light->state = state;

	Ref<SceneLight> sceneLight = CreateRef<SceneLight>(light);
	sceneLight->uuid = uuid;
	const YAML::Node& attchNode = node["Attachments"];
	if (attchNode)
	{
		YAML::const_iterator it;
		for (it = attchNode.begin(); it != attchNode.end(); it++)
		{
			const YAML::Node& child = (*it);
			Ref<FlickerAttachment> flicker = CreateRef<FlickerAttachment>(light);
			sceneLight->attachements.push_back(flicker);
			/*const YAML::Node& type = child["Flicker"];
			if (type)
			{
				Ref<FlickerAttachment> flicker = CreateRef<FlickerAttachment>(light);
				flicker->Load(child["Flicker"]);
				sceneLight->attachements.push_back(flicker);
			}*/
		}
	}
	return sceneLight;
}