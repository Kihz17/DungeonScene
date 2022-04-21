#pragma once

#include "pch.h"
#include "Texture.h"
#include "Shader.h"

#include <glm/glm.hpp>

class HeightMapTexture : public Texture
{
public:
	HeightMapTexture( const std::string& path, TextureFilterType filterType, TextureWrapType wrapType, float scale);
	virtual ~HeightMapTexture();

	virtual void Bind(uint32_t slot = 0) const override;
	virtual void UnBind(uint32_t slot = 0) const override;

	inline virtual std::string GetPath() const override { return this->path; };

	inline virtual const float& GetScale() const { return this->scale; }
	inline virtual void SetOffset(const glm::vec3& offset) { this->offset = offset; }
	inline virtual const glm::vec3& GetOffset() const { return this->offset; }

	static void InitializeUniforms(Ref<Shader> shader);
private:
	GLuint ID;
	std::string path;
	glm::vec3 offset;
	float scale;

	static GLuint useHeightMapUniform;
	static GLuint heightMapTextureUniform;
	static GLuint heightMapScaleUniform;
	static GLuint heightMapOffsetUniform;
};