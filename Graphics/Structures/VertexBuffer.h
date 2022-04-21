#pragma once

#include "pch.h"
#include "VertexInformation.h"
#include "GLCommon.h"

class VertexBuffer
{
public:
	VertexBuffer(float* vertices, uint32_t size);
	virtual ~VertexBuffer();

	void Bind() const;
	void Unbind() const;

	void SetData(const void* data, uint32_t size);

	inline const BufferLayout& GetLayout() const { return this->layout; }
	inline void SetLayout(const BufferLayout& layout) { this->layout = layout; }

private:
	GLuint ID;
	BufferLayout layout;
};