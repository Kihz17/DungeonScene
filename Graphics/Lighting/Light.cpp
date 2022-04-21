#include "Light.h"
#include "YAMLOverloads.h"

#include <iostream>
#include <sstream>

std::vector<LightUniform> Light::lightUniforms;

Light::Light(int index) :
	index(index), 
	position(0.0f, 0.0f, 0.0f, 0.0f), 
	diffuse(1.0f, 1.0f, 1.0f, 1.0f),
	specular(1.0f, 1.0f, 1.0f, 1.0f),
	attenuation(0.0f, 0.1f, 0.01f, 100000.0f),
	direction(0.0f, -1.0f, 0.0f, 0.0f),
	innerAngle(0.0f),
	outerAngle(0.0f),
	state(true)
{
	this->lightType = POINT;
}

Light::~Light()
{

}

void Light::EditPosition(float x, float y, float z, float w)
{
	this->position = glm::vec4(x, y, z, w);
	glUniform4f(lightUniforms[this->index].positionLocation, this->position.x, this->position.y, this->position.z, this->position.w);
}

void Light::EditDiffuse(float x, float y, float z, float w)
{
	this->diffuse = glm::vec4(x, y, z, w);
	glUniform4f(lightUniforms[this->index].diffuseLocation, this->diffuse.x, this->diffuse.y, this->diffuse.z, this->diffuse.w);
}

void Light::EditSpecular(float r, float g, float b, float power)
{
	this->specular = glm::vec4(r, g, b, power);
	glUniform4f(lightUniforms[this->index].specularLocation, this->specular.x, this->specular.y, this->specular.z, this->specular.w);
}

void Light::EditAttenuation(float constant, float linear, float quadratic, float distanceCutOff)
{
	this->attenuation = glm::vec4(constant, linear, quadratic, distanceCutOff);
	glUniform4f(lightUniforms[this->index].attenuationLocation, this->attenuation.x, this->attenuation.y, this->attenuation.z, this->attenuation.w);
}

void Light::EditDirection(float x, float y, float z, float w)
{
	this->direction = glm::vec4(x, y, z, w);
	glUniform4f(lightUniforms[this->index].directionLocation, this->direction.x, this->direction.y, this->direction.z, this->direction.w);
}

void Light::EditLightType(LightType lightType, float innerAngle, float outerAngle)
{
	this->lightType = lightType;
	this->innerAngle = innerAngle;
	this->outerAngle = outerAngle;
	glUniform4f(lightUniforms[this->index].param1Location, (float)this->lightType, this->innerAngle, this->outerAngle, 1.0f);
}

void Light::EditState(bool on)
{
	this->state = on;
	glUniform4f(lightUniforms[this->index].param2Location, on ? (float) GL_TRUE : (float) GL_FALSE, 1.0f, 1.0f, 1.0f);
}

void Light::SendToShader()
{	
	glUniform4f(lightUniforms[this->index].positionLocation, this->position.x, this->position.y, this->position.z, this->position.w);
	glUniform4f(lightUniforms[this->index].diffuseLocation, this->diffuse.x, this->diffuse.y, this->diffuse.z, this->diffuse.w);
	glUniform4f(lightUniforms[this->index].specularLocation, this->specular.x, this->specular.y, this->specular.z, this->specular.w);
	glUniform4f(lightUniforms[this->index].attenuationLocation, this->attenuation.x, this->attenuation.y, this->attenuation.z, this->attenuation.w);
	glUniform4f(lightUniforms[this->index].directionLocation, this->direction.x, this->direction.y, this->direction.z, this->direction.w);
	glUniform4f(lightUniforms[this->index].param1Location, (float)this->lightType, this->innerAngle, this->outerAngle, 1.0f);

	if (this->state)
	{
		glUniform4f(lightUniforms[this->index].param2Location, (float) GL_TRUE, 1.0f, 1.0f, 1.0f);
	}
	else
	{
		glUniform4f(lightUniforms[this->index].param2Location, (float) GL_FALSE, 1.0f, 1.0f, 1.0f);
	}
}

void Light::Save(YAML::Emitter& emitter) const
{
	emitter << YAML::BeginMap;
	emitter << YAML::Key << "Index" << YAML::Value << this->index;
	emitter << YAML::Key << "Position" << YAML::Value << this->position;
	emitter << YAML::Key << "Diffuse" << YAML::Value << this->diffuse;
	emitter << YAML::Key << "Specular" << YAML::Value << this->specular;
	emitter << YAML::Key << "Attenuation" << YAML::Value << this->attenuation;
	emitter << YAML::Key << "Direction" << YAML::Value << this->direction;
	emitter << YAML::Key << "LightType" << YAML::Value << LightTypeToString(this->lightType);
	emitter << YAML::Key << "OuterAngle" << YAML::Value << this->outerAngle;
	emitter << YAML::Key << "InnerAngle" << YAML::Value << this->innerAngle;
	emitter << YAML::Key << "State" << YAML::Value << this->state;
	emitter << YAML::EndMap;
}

Ref<Light> Light::StaticLoad(YAML::Node& node)
{
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

	return light;
}

void Light::InitializeUniforms(Ref<Shader> shader)
{
	GLuint shaderID = shader->GetID();

	Light::lightUniforms.resize(Light::numOfLights);

	for (int i = 0; i < numOfLights; i++)
	{
		LightUniform& lightUniform = Light::lightUniforms[i];
		{
			std::stringstream ss;
			ss << "lightArray[" << i << "].position";
			lightUniform.positionLocation = glGetUniformLocation(shaderID, ss.str().c_str());
		}
		{
			std::stringstream ss;
			ss << "lightArray[" << i << "].diffuse";
			lightUniform.diffuseLocation = glGetUniformLocation(shaderID, ss.str().c_str());
		}
		{
			std::stringstream ss;
			ss << "lightArray[" << i << "].specular";
			lightUniform.specularLocation = glGetUniformLocation(shaderID, ss.str().c_str());
		}
		{
			std::stringstream ss;
			ss << "lightArray[" << i << "].attenuation";
			lightUniform.attenuationLocation = glGetUniformLocation(shaderID, ss.str().c_str());
		}
		{
			std::stringstream ss;
			ss << "lightArray[" << i << "].direction";
			lightUniform.directionLocation = glGetUniformLocation(shaderID, ss.str().c_str());
		}
		{
			std::stringstream ss;
			ss << "lightArray[" << i << "].param1";
			lightUniform.param1Location = glGetUniformLocation(shaderID, ss.str().c_str());
		}
		{
			std::stringstream ss;
			ss << "lightArray[" << i << "].param2";
			lightUniform.param2Location = glGetUniformLocation(shaderID, ss.str().c_str());
		}
	}
}

float Light::CalcApproxDistFromAtten(float targetLightLevel, float accuracy, float infiniteDistance,
	float constAttenuation,
	float linearAttenuation,
	float quadraticAttenuation,
	unsigned int maxIterations)
{
	// See if the accuracy being set it too big for the targetLightLevel, unless targetLightLevel is actually zero (0.0f)
	// If it's actually zero, then adjusting the accuracy to a tenth of zero would give... wait for it...
	//	zero, and we would max out the iterations
	if (targetLightLevel != 0.0f)
	{
		if ((accuracy * 10.0f) >= targetLightLevel * 10.0f)
		{	// Adjust the accuracy by a hundreth
			accuracy = targetLightLevel / 10.0f;
		}
	}

	float targetLightLevelLow = targetLightLevel - accuracy;
	float targetLightLevelHigh = targetLightLevel + accuracy;

	constexpr float defaultInfiniteDistance = 10000.0f;
	// See if we're getting a value at infinite. i.e. at 'infinite distance', is the light level too high already
	if (CalcDiffuseFromAttenByDistance(defaultInfiniteDistance,
		constAttenuation, linearAttenuation, quadraticAttenuation,
		accuracy) > targetLightLevelHigh)
	{	// Yes, so we can never get down to this light level
		return defaultInfiniteDistance;
	}

	// There is a light level somewhere between a distance of 0.0 to DEFAULTINFINITEDISTANCE
	// Set initial guesses
	float distanceGuessLow = 0.0f;
	float distanceGuessHigh = defaultInfiniteDistance;

	unsigned int iterationCount = 0;
	while (iterationCount < maxIterations)
	{
		// Pick a distance between the high and low
		float curDistanceGuess = ((distanceGuessHigh - distanceGuessLow) / 2.0f) + distanceGuessLow;
		float curDiffuseAtGuessDistance = CalcDiffuseFromAttenByDistance(curDistanceGuess, constAttenuation, linearAttenuation, quadraticAttenuation, 0.0001f);

		// Could be three possibilities: too low, too high, or in between
		if (curDiffuseAtGuessDistance < targetLightLevelLow)
		{	// Light is too dark, so distance is to HIGH. Reduce and guess again.
			distanceGuessHigh = curDistanceGuess;		// Lower the high limit for the guesses
		}
		else if (curDiffuseAtGuessDistance > targetLightLevelHigh)
		{	// Light is too bright, so distance is to LOW. Increase and guess again
			distanceGuessLow = curDistanceGuess;
		}
		else
		{	// Nailed it! Light level is within range, so return this distance
			return curDistanceGuess;
		}

		iterationCount++;

	}

	// If we are here, then we ran out of iterations.
	// Pick a distance between the low and high
	float distance = (distanceGuessHigh - distanceGuessLow) / 2.0f;

	return distance;
}

float Light::CalcDiffuseFromAttenByDistance(float distance,
	float constAttenuation,
	float linearAttenuation,
	float quadraticAttenuation,
	float zeroThreshold)
{
	float diffuse = 1.0f;		// Assume full brightness

	float denominator = constAttenuation +
		linearAttenuation * distance +
		quadraticAttenuation * distance * distance;

	if (denominator <= zeroThreshold)
	{	// Let's just say it's zero, shall we?
		diffuse = 1.0f;
	}
	else
	{
		float atten = 1.0f / denominator;
		diffuse *= atten;
		if (diffuse > 1.0f)
		{
			diffuse = 1.0f;
		}
	}

	return diffuse;
}