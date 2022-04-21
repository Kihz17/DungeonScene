#pragma once

#include "VertexBuffer.h"
#include "IndexBuffer.h"

#include <vector>

class VertexArrayObject
{
public:
	VertexArrayObject();
	virtual ~VertexArrayObject();

	void Bind() const;
	void Unbind() const ;

	void AddVertexBuffer(const Ref<VertexBuffer>& vbo);
	void SetIndexBuffer(const Ref<IndexBuffer>& ebo);

	inline virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const { return this->vertexBuffers; }
	inline virtual const Ref<IndexBuffer>& GetIndexBuffer() const { return this->indexBuffer; }

private:
	GLuint ID; // Holds the ID to our VAO
	GLuint VBOIndex; // Holds the current index of out VBO

	std::vector<Ref<VertexBuffer>> vertexBuffers;
	Ref<IndexBuffer> indexBuffer;
};