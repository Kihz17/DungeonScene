#pragma once

#include "pch.h"

#include "GLCommon.h"

class IndexBuffer
{
public:
	IndexBuffer(uint32_t* indices, uint32_t count);
	virtual ~IndexBuffer();

	void Bind() const;;
	void Unbind() const;

	inline uint32_t GetCount() const { return this->count; }

private:
	GLuint ID;
	GLuint count; // Number of faces
};