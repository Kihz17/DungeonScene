#pragma once

#include "pch.h"
#include "GLCommon.h"
#include "Shader.h"
#include "Serializable.h"

#include <glm/glm.hpp>
#include <glm/vec3.hpp> 
#include <glm/vec4.hpp> 

#include <iostream>

struct LightUniform
{
	GLuint positionLocation = 0;
	GLuint diffuseLocation = 0;
	GLuint specularLocation = 0;
	GLuint attenuationLocation = 0;
	GLuint directionLocation = 0;
	GLuint param1Location = 0; // vec4(lightType, innerAngle, outerAngle, ???)
	GLuint param2Location = 0; // vec4(isLightOn, ???, ???, ???)
};

class Light 
{
public:
	Light(int index);
	~Light();

	enum LightType
	{
		POINT = 0,
		SPOT = 1,
		DIRECTIONAL = 2,
	};

	// Modifies the position of this light
	void EditPosition(float x, float y, float z, float w);

	// Modifies the diffuse of this light
	void EditDiffuse(float x, float y, float z, float w);

	// Modifies the specular color and power of this light
	void EditSpecular(float r, float g, float b, float power);

	// Modifies the attenuation this light
	void EditAttenuation(float constant, float linear, float quadratic, float distanceCutOff);

	// Modifies the direction of this light (Does nothing if we aren't a SPOT/DIRECTIONAL light)
	void EditDirection(float x, float y, float z, float w);

	// Modifies the type of light we are
	void EditLightType(LightType lightType, float innerAngle, float outerAngle);

	// Modifies if the light is on or off
	void EditState(bool on);

	// Will copy this light's information to the shader (WARNING: Make sure you call SetupUniforms() before calling this to ensure that this object's uniform locations are set.)
	void SendToShader();

	virtual void Save(YAML::Emitter& emitter) const;

	static Ref<Light> StaticLoad(YAML::Node& node);

	static void InitializeUniforms(Ref<Shader> shader);
	static float CalcApproxDistFromAtten(float targetLightLevel, float accuracy, float infiniteDistance, 
		float constAttenuation = 0.1f, 
		float linearAttenuation = 0.1f,
		float quadraticAttenuation = 0.1, 
		unsigned int maxIterations = 50);

	static float CalcDiffuseFromAttenByDistance(float distance,
		float constAttenuation,
		float linearAttenuation,
		float quadraticAttenuation,
		float zeroThreshold = 0.0001f);

	// Lighting members
	glm::vec4 position;
	glm::vec4 diffuse;
	glm::vec4 specular;
	glm::vec4 attenuation;
	glm::vec4 direction;
	LightType lightType;
	float innerAngle;
	float outerAngle;
	bool state;

	const unsigned int index; // Stores the index the light is at in the fragment shader

private:
	friend class Scene;

	// Uniforms
	static std::vector<LightUniform> lightUniforms;
	static const int numOfLights = 100;
};

static std::string LightTypeToString(Light::LightType lightType)
{
	switch (lightType)
	{
	case Light::LightType::DIRECTIONAL:
		return "DIRECTIONAL";
	case Light::LightType::SPOT:
		return "SPOT";
	case Light::LightType::POINT:
		return "POINT";
	}

	std::cout << "Light type " << " not found!" << std::endl;
	return "";
}

static Light::LightType StringToLightType(const std::string& type)
{
	if (type == "DIRECTIONAL") return Light::LightType::DIRECTIONAL;
	else if (type == "SPOT") return Light::LightType::SPOT;
	else if (type == "POINT") return Light::LightType::POINT;

	std::cout << "Light type '" << type << "' not found!" << std::endl;
	return Light::LightType::DIRECTIONAL;
}

