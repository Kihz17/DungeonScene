#pragma once

#include "Texture.h"
#include "Shader.h"

#include <string>

class AlphaTexture : public Texture
{
public:
	AlphaTexture(const std::string& path, TextureFilterType filterType, TextureWrapType wrapType, bool genMipMaps = true);
	virtual ~AlphaTexture();

	virtual void Bind(uint32_t slot = 0) const override;
	virtual void UnBind(uint32_t slot = 0) const override;

	inline virtual std::string GetPath() const override { return this->path; };

	inline virtual bool IsGenMipMaps() const { return this->genMipMaps; }

	static void InitializeUniforms(Ref<Shader>shader);

private:
	GLuint ID;
	std::string path;
	bool genMipMaps;

	static GLuint isAlphaTextureUniform;
	static GLuint alphaTextureUniform;
};