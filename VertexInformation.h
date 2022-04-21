#pragma once

#include <string>

#include <assimp/scene.h> 
#include <iostream>
#include <vector>

// Represents the various data types that can be used inside a shader
enum class ShaderDataType
{
	None = 0,
	Float, Float2, Float3, Float4, // Float Types
	Int, Int2, Int3, Int4, // Int Types
	Mat3x3, Mat4x4, // Matrix Types,
	Bool
};

// Gets the size (in bytes) of a ShaderDataType
static size_t GetShaderDataTypeSize(ShaderDataType type)
{
	switch (type)
	{
	case ShaderDataType::Float:
		return 4; // A float is 4 bytes
	case ShaderDataType::Float2:
		return 4 * 2;
	case ShaderDataType::Float3:
		return 4 * 3;
	case ShaderDataType::Float4:
		return 4 * 4;
	case ShaderDataType::Int:
		return 4; // And int is 4 bytes
	case ShaderDataType::Int2:
		return 4 * 2;
	case ShaderDataType::Int3:
		return 4 * 3;
	case ShaderDataType::Int4:
		return 4 * 4;
	case ShaderDataType::Mat3x3:
		return 4 * 3 * 3; // Matrix consists of floats which are 4 bytes
	case ShaderDataType::Mat4x4:
		return 4 * 4 * 4;
	case ShaderDataType::Bool:
		return 1;
	default:
		std::cout << "Invalid ShaderDataType!" << std::endl;
		return 0;
	}
}

// Represents a buffer element that will be contained in a BufferLayout object to specify the layout of a VBO to the GPU
struct BufferElement
{
	BufferElement() = default;
	BufferElement(ShaderDataType dataType, const std::string& name, bool normalized = false)
		: name(name), shaderDataType(dataType), size(GetShaderDataTypeSize(dataType)), offset(0), normalized(false) {}

	size_t NumberOfComponents() const
	{

		switch (this->shaderDataType)
		{
		case ShaderDataType::Float:
			return 1;
		case ShaderDataType::Float2:
			return 2;
		case ShaderDataType::Float3:
			return 3;
		case ShaderDataType::Float4:
			return 4;
		case ShaderDataType::Int:
			return 1;
		case ShaderDataType::Int2:
			return 2;
		case ShaderDataType::Int3:
			return 3;
		case ShaderDataType::Int4:
			return 4;
		case ShaderDataType::Mat3x3:
			return 3; // 3x Float3
		case ShaderDataType::Mat4x4:
			return 4; // 4x Float3
		case ShaderDataType::Bool:
			return 1;
		default:
			std::cout << "Invalid ShaderDataType!" << std::endl;
			return 0;
		}
	}

	std::string name;
	ShaderDataType shaderDataType;
	size_t size;
	size_t offset;
	bool normalized;
};

// Represents a buffer layout that will be passed to the GPU to describe the layout of our VBO
class BufferLayout
{
public:
	BufferLayout() = default;

	BufferLayout(std::initializer_list<BufferElement> bufferElements) : bufferElements(bufferElements)
	{
		// Initialize elements with offset and stride
		size_t offset = 0;
		this->stride = 0;
		for (BufferElement& element : this->bufferElements)
		{
			element.offset = offset;
			offset += element.size;
			this->stride += element.size;
		}
	}

	inline uint32_t GetStride() const { return this->stride; }
	inline const std::vector<BufferElement>& GetElements() const { return this->bufferElements; }

	inline std::vector<BufferElement>::iterator begin() { return this->bufferElements.begin(); }
	inline std::vector<BufferElement>::iterator end() { return this->bufferElements.end(); }
	inline std::vector<BufferElement>::const_iterator begin() const { return this->bufferElements.begin(); }
	inline std::vector<BufferElement>::const_iterator end() const { return this->bufferElements.end(); }

private:
	std::vector<BufferElement> bufferElements;
	uint32_t stride = 0; // Represents the offset of bytes between consecutive vertex attributes
};