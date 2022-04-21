#pragma once

#include "GLCommon.h"

#include <string>

enum class TextureFilterType
{
	Linear,
	Nearest
};

enum class TextureWrapType
{
	Clamp,
	Repeat
};

class Texture
{
public:
	Texture(const std::string& name, const std::string& path, TextureFilterType filterType, TextureWrapType wrapType, bool genMipMaps = true);

	GLuint GetID() const;

	void Bind(uint32_t textureSlot = 0);

	inline std::string GetName() { return this->name; }

	inline std::string GetPath() { return this->path; }

private:
	friend class TextureManager;
	friend class EnvironmentMap;

	GLuint ID;
	std::string name;
	std::string path;
};