#pragma once

#include "Serializable.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "GLCommon.h"

#include <string>
#include <stdint.h>

class EnvironmentMap 
{
public:
	EnvironmentMap(Ref<Mesh> mesh, const std::string& posXFile, const std::string& negXFile, 
		const std::string& posYFile, const std::string& negYFile,
		const std::string& posZFile, const std::string& negZFile, 
		bool mipmaps = true, bool seamless = true);

	virtual ~EnvironmentMap();

	void Draw(Ref<Shader> shader, const glm::vec3& position, const glm::vec3& scale);

	inline std::string GetPosXFile() { return this->posXFile; }
	inline std::string GetNegXFile() { return this->negXFile; }
	inline std::string GetPosYFile() { return this->posYFile; }
	inline std::string GetNegYFile() { return this->negYFile; }
	inline std::string GetPosZFile() { return this->posZFile; }
	inline std::string GetNegZFile() { return this->negZFile; }

	virtual void Save(YAML::Emitter& emitter) const;

	static Ref<EnvironmentMap> StaticLoad(const YAML::Node& node);
private:
	void LoadUniforms(Ref<Shader> shader);

	GLuint ID;
	std::string posXFile;
	std::string negXFile;
	std::string posYFile;
	std::string negYFile;
	std::string posZFile;
	std::string negZFile;
	bool mipMaps;
	bool seamless;

	int width;
	int height;
	Ref<Mesh> mesh;

	bool loadedUniforms;
	GLuint isSkyBoxUniform = 0;
	GLuint cubeMapRatiosUniform = 0;
	GLuint cubeSamplerUniform0 = 0;
	GLuint cubeSamplerUniform1 = 0;
	GLuint cubeSamplerUniform2 = 0;
	GLuint cubeSamplerUniform3 = 0;
};