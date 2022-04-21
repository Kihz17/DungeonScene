#include "EnvironmentMap.h"
#include "MeshManager.h"

#include <SOIL2.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

EnvironmentMap::EnvironmentMap(Ref<Mesh> mesh, const std::string& posXFile, const std::string& negXFile, 
	const std::string& posYFile, const std::string& negYFile, 
	const std::string& posZFile, const std::string& negZFile, 
	bool mipmaps, bool seamless)
	: posXFile(posXFile), 
	negXFile(negXFile), 
	posYFile(posYFile), 
	negYFile(negYFile), 
	posZFile(posZFile), 
	negZFile(negZFile), 
	mesh(mesh),
	mipMaps(mipmaps),
	seamless(seamless),
	loadedUniforms(false)
{
	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &this->ID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->ID);

	// Wrapping
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// Filters
	if (mipmaps)
	{
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (seamless)
	{
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	}

	// Load image data
	int width, height, channels;

	// Positive X
	uint8_t* posXData = SOIL_load_image(posXFile.c_str(), &width, &height, &channels, SOIL_LOAD_RGB);
	if (!posXData)
	{
		std::cout << "Failed to load positive X data!" << std::endl;
		return;
	}

	GLenum imageFormat = channels == 4 ? GL_RGBA : GL_RGB;
	glTextureStorage2D(GL_TEXTURE_CUBE_MAP, 0, GL_RGBA8, width, height);

	this->width = width;
	this->height = height;

	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA8, this->width, this->height, 0, imageFormat, GL_UNSIGNED_BYTE, posXData);

	// Negative X
	uint8_t* negXData = SOIL_load_image(negXFile.c_str(), &width, &height, &channels, SOIL_LOAD_RGB);
	if (width != this->width || height != this->height)
	{
		std::cout << "Cube map image sizes are differrent!";
		return;
	}
	else if (!negXData)
	{
		std::cout << "Failed to load negative X data!" << std::endl;
		return;
	}

	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA8, this->width, this->height, 0, imageFormat, GL_UNSIGNED_BYTE, negXData);

	// Postive Y
	uint8_t* posYData = SOIL_load_image(posYFile.c_str(), &width, &height, &channels, SOIL_LOAD_RGB);
	if (width != this->width || height != this->height)
	{
		std::cout << "Cube map image sizes are differrent!";
		return;
	}
	else if (!posYData)
	{
		std::cout << "Failed to load positive Y data!" << std::endl;
		return;
	}

	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA8, this->width, this->height, 0, imageFormat, GL_UNSIGNED_BYTE, posYData);

	// Negtive Y
	uint8_t* negYData = SOIL_load_image(negYFile.c_str(), &width, &height, &channels, SOIL_LOAD_RGB);
	if (width != this->width || height != this->height)
	{
		std::cout << "Cube map image sizes are differrent!";
		return;
	}
	else if (!negYData)
	{
		std::cout << "Failed to load negative Y data!" << std::endl;
		return;
	}

	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA8, this->width, this->height, 0, imageFormat, GL_UNSIGNED_BYTE, negYData);

	// Positive Z
	uint8_t* posZData = SOIL_load_image(posZFile.c_str(), &width, &height, &channels, SOIL_LOAD_RGB);
	if (width != this->width || height != this->height)
	{
		std::cout << "Cube map image sizes are differrent!";
		return;
	}
	else if (!posZData)
	{
		std::cout << "Failed to load positive Z data!" << std::endl;
		return;
	}

	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA8, this->width, this->height, 0, imageFormat, GL_UNSIGNED_BYTE, posZData);

	// Negative Z
	uint8_t* negZData = SOIL_load_image(negZFile.c_str(), &width, &height, &channels, SOIL_LOAD_RGB);
	if (width != this->width || height != this->height)
	{
		std::cout << "Cube map image sizes are differrent!";
		return;
	}
	else if (!negZData)
	{
		std::cout << "Failed to load negative Z data!" << std::endl;
		return;
	}

	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA8, this->width, this->height, 0, imageFormat, GL_UNSIGNED_BYTE, negZData);

	if (mipmaps)
	{
		glGenerateMipmap(this->ID);
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	SOIL_free_image_data(posXData);
	SOIL_free_image_data(negXData);
	SOIL_free_image_data(posYData);
	SOIL_free_image_data(negYData);
	SOIL_free_image_data(posZData);
	SOIL_free_image_data(negZData);

	std::cout << "Environment map loaded successfuly!" << std::endl;
}

EnvironmentMap::~EnvironmentMap()
{
	glDeleteTextures(1, &this->ID);
}

void EnvironmentMap::Draw(Ref<Shader> shader, const glm::vec3& position, const glm::vec3& scale)
{
	if (!this->loadedUniforms)
	{
		LoadUniforms(shader);
	}

	shader->Bind();

	glUniform1f(this->isSkyBoxUniform, (GLfloat) GL_TRUE);
	glUniform4f(this->cubeMapRatiosUniform, 1.0f, 1.0f, 1.0f, 1.0f);

	glm::mat4 matModel = glm::mat4(1.0f);
	matModel *= glm::translate(glm::mat4(1.0f), position);
	matModel *= glm::scale(glm::mat4(1.0f), scale);
	glm::mat4 matInvTransposeModel = glm::inverse(glm::transpose(matModel));

	shader->SetMat4x4("matModel", matModel);
	shader->SetMat4x4("matModelInverseTranspose", matInvTransposeModel);
	
	GLuint textureUnit = 40; // Quick hack to ensure that our cube map doesn't clash with 2d textures

	glBindTextureUnit(textureUnit, this->ID);

	glUniform1i(this->cubeSamplerUniform0, textureUnit);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	this->mesh->GetVertexArray()->Bind();
	glDrawElements(GL_TRIANGLES, this->mesh->GetFaces().size() * 3, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glUniform1f(this->isSkyBoxUniform, (GLfloat) GL_FALSE);
}

void EnvironmentMap::LoadUniforms(Ref<Shader> shader)
{
	this->isSkyBoxUniform = glGetUniformLocation(shader->GetID(), "isSkyBox");
	this->cubeMapRatiosUniform = glGetUniformLocation(shader->GetID(), "cubeMapRatios");
	this->cubeSamplerUniform0 = glGetUniformLocation(shader->GetID(), "cubeMap0");
	this->cubeSamplerUniform1 = glGetUniformLocation(shader->GetID(), "cubeMap1");
	this->cubeSamplerUniform2 = glGetUniformLocation(shader->GetID(), "cubeMap2");
	this->cubeSamplerUniform3 = glGetUniformLocation(shader->GetID(), "cubeMap3");
	this->loadedUniforms = true;
}

void EnvironmentMap::Save(YAML::Emitter& emitter) const
{
	emitter << YAML::BeginMap;
	emitter << YAML::Key << "PosXPath" << YAML::Value << SerializeUtils::SavePath(this->posXFile);
	emitter << YAML::Key << "NegXPath" << YAML::Value << SerializeUtils::SavePath(this->negXFile);
	emitter << YAML::Key << "PosYPath" << YAML::Value << SerializeUtils::SavePath(this->posYFile);
	emitter << YAML::Key << "NegYPath" << YAML::Value << SerializeUtils::SavePath(this->negYFile);
	emitter << YAML::Key << "PosZPath" << YAML::Value << SerializeUtils::SavePath(this->posZFile);
	emitter << YAML::Key << "NegZPath" << YAML::Value << SerializeUtils::SavePath(this->negZFile);
	emitter << YAML::Key << "MipMaps" << YAML::Value << this->mipMaps;
	emitter << YAML::Key << "Seamless" << YAML::Value << this->seamless;
	emitter << YAML::Key << "MeshPath" << YAML::Value << SerializeUtils::SavePath(this->mesh->GetPath());
	emitter << YAML::EndMap;
}

Ref<EnvironmentMap> EnvironmentMap::StaticLoad(const YAML::Node& node)
{
	const YAML::Node& test = node["PosXPath"];
	test.as<std::string>();
	std::string posXPath = SerializeUtils::LoadPath(node["PosXPath"].as<std::string>());
	std::string negXPath = SerializeUtils::LoadPath(node["NegXPath"].as<std::string>());
	std::string posYPath = SerializeUtils::LoadPath(node["PosYPath"].as<std::string>());
	std::string negYPath = SerializeUtils::LoadPath(node["NegYPath"].as<std::string>());
	std::string posZPath = SerializeUtils::LoadPath(node["PosZPath"].as<std::string>());
	std::string negZPath = SerializeUtils::LoadPath(node["NegZPath"].as<std::string>());
	bool mipMaps = node["MipMaps"].as<bool>();
	bool seamless = node["Seamless"].as<bool>();
	std::string meshPath = SerializeUtils::LoadPath(node["MeshPath"].as<std::string>());
	Ref<Mesh> mesh = MeshManager::LoadMesh(meshPath);
	return CreateRef<EnvironmentMap>(mesh, posXPath, negXPath, posYPath, negYPath, posZPath, negZPath, mipMaps, seamless);
}