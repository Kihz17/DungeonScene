#include "VertexArrayObject.h"

VertexArrayObject::VertexArrayObject()
	: VBOIndex(0)
{
	glCreateVertexArrays(1, &this->ID);
}

VertexArrayObject::~VertexArrayObject()
{
	glDeleteVertexArrays(1, &this->ID);
}

void VertexArrayObject::Bind() const
{
	glBindVertexArray(this->ID);
}

void VertexArrayObject::Unbind() const
{
	glBindVertexArray(0);
}

void VertexArrayObject::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
{
	if (vertexBuffer->GetLayout().GetElements().empty())
	{
		std::cout << "VBO doesn't have a layout!" << std::endl;
		return;
	}

	Bind();
	vertexBuffer->Bind();

	const BufferLayout& layout = vertexBuffer->GetLayout();
	for (const BufferElement& element : layout)
	{
		switch (element.shaderDataType)
		{
		case ShaderDataType::Float:
		case ShaderDataType::Float2:
		case ShaderDataType::Float3:
		case ShaderDataType::Float4:
		{
			glEnableVertexAttribArray(this->VBOIndex); // Bind the current VBO
			// Tell OpenGL about the layout of our data
			glVertexAttribPointer(this->VBOIndex,
				element.NumberOfComponents(), // How many components do we have.
				GL_FLOAT, // The type of data we are passing
				element.normalized ? GL_TRUE : GL_FALSE, // Should the data be normalized?
				layout.GetStride(), // The offset between vertex attributes
				(GLvoid*)element.offset); // The offset of the first component of the vertex attribute
			this->VBOIndex++;
			break;
		}
		case ShaderDataType::Int:
		case ShaderDataType::Int2:
		case ShaderDataType::Int3:
		case ShaderDataType::Int4:
		case ShaderDataType::Bool:
		{
			glEnableVertexAttribArray(this->VBOIndex); // Bind the current VBO
			// Tell OpenGL about the layout of our data
			glVertexAttribPointer(this->VBOIndex,
				element.NumberOfComponents(), // How many components do we have. In most cases this will be 3 because we like to draw triangles
				GL_INT, // The type of data we are passing
				GL_FALSE, // We can't normalize integers
				layout.GetStride(), // The offset between vertex attributes
				(GLvoid*)element.offset); // The offset of the first component of the vertex attribute
			this->VBOIndex++;
			break;
		}
		case ShaderDataType::Mat3x3:
		case ShaderDataType::Mat4x4:
		{
			std::cout << "Matrices have no implementation." << std::endl; // Do this later. It really isn't important to me right now because I feel like I will never use this
			break;
		}
		default:
			std::cout << "Invalid ShaderDataType!" << std::endl;;
		}
	}

	this->vertexBuffers.push_back(vertexBuffer);
}

void VertexArrayObject::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
{
	Bind();
	indexBuffer->Bind();
	this->indexBuffer = indexBuffer;
}