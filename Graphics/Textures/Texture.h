#pragma once

#include "GLCommon.h"
#include "Serializable.h"

#include <string>
#include <iostream>

enum class TextureFilterType
{
	None = 0,
	Linear,
	Nearest
};

enum class TextureWrapType
{
	None = 0,
	Clamp,
	Repeat
};

enum class TextureType
{
	None = 0,
	Diffuse,
	Heightmap,
	Discard,
	Alpha
};

class Texture
{
public:
	Texture(TextureFilterType filterType, TextureWrapType wrapType, TextureType textureType) : filterType(filterType), wrapType(wrapType), textureType(textureType) {}
	virtual ~Texture() = default;

	virtual void Bind(uint32_t slot = 0) const = 0;
	virtual void UnBind(uint32_t slot = 0) const = 0;

	inline virtual TextureType GetType() const { return this->textureType; }
	inline virtual TextureFilterType GetFilterType() const { return this->filterType; };
	inline virtual TextureWrapType GetWrapType() const { return this->wrapType; };

	virtual std::string GetPath() const = 0;

private:
	TextureType textureType;
	TextureFilterType filterType;
	TextureWrapType wrapType;
};

static std::string TextureTypeToString(TextureType type)
{
	switch (type)
	{
	case TextureType::Diffuse:
		return "Diffuse";
	case TextureType::Heightmap:
		return "Heightmap";
	case TextureType::Discard:
		return "Discard";
	case TextureType::Alpha:
		return "Alpha";
	}

	std::cout << "Texture type not found!" << std::endl;
	return "";
}
static TextureType StringToTextureType(const std::string& value)
{
	if (value == "Diffuse") return TextureType::Diffuse;
	else if (value == "Heightmap") return TextureType::Heightmap;
	else if (value == "Discard") return TextureType::Discard;
	else if (value == "Alpha") return TextureType::Alpha;

	std::cout << "Texture type '" << value << "' not found!" << std::endl;
	return TextureType::None;
}

static std::string FilterTypeToString(TextureFilterType type)
{
	switch (type)
	{
	case TextureFilterType::Linear:
		return "Linear";
	case TextureFilterType::Nearest:
		return "Nearest";
	}

	std::cout << "Filter type not found!" << std::endl;
	return "";
}
static TextureFilterType StringToFilterType(const std::string& value)
{
	if (value == "Linear") return TextureFilterType::Linear;
	else if (value == "Nearest") return TextureFilterType::Nearest;

	std::cout << "Texture type '" << value << "' not found!" << std::endl;
	return TextureFilterType::None;
}

static std::string WrapTypeToString(TextureWrapType type)
{
	switch (type)
	{
	case TextureWrapType::Clamp:
		return "Clamp";
	case TextureWrapType::Repeat:
		return "Repeat";
	}

	std::cout << "Texture type not found!" << std::endl;
	return "";
}
static TextureWrapType StringToWrapType(const std::string& value)
{
	if (value == "Clamp") return TextureWrapType::Clamp;
	else if (value == "Repeat") return TextureWrapType::Repeat;

	std::cout << "Texture Wrap type '" << value << "' not found!" << std::endl;
	return TextureWrapType::None;
}
