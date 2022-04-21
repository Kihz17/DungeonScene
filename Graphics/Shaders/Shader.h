#pragma once

#include "GLCommon.h"
#include <glm/glm.hpp>

#include <string>
#include <vector>

class Shader
{
public:		
	Shader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath);
	Shader(const std::string& name, const std::vector<std::string>& vertexSrc, const std::vector<std::string>& fragmentSrc);
	virtual ~Shader();

	inline virtual GLuint GetID() { return this->ID; }
	virtual void Bind() const;
	virtual void Unbind() const;

	virtual void SetInt(const std::string & name, int value);
	virtual void SetIntArray(const std::string & name, int* values, uint32_t count);
	virtual void SetFloat(const std::string & name, float value);
	virtual void SetFloat2(const std::string & name, const glm::vec2 & value);
	virtual void SetFloat3(const std::string & name, const glm::vec3 & value);
	virtual void SetFloat4(const std::string & name, const glm::vec4 & value);
	virtual void SetMat4x4(const std::string & name, const glm::mat4 & value);

	inline virtual const std::string& GetName() const { return this->name; };

private:
	GLuint ID;
	std::string name;
};