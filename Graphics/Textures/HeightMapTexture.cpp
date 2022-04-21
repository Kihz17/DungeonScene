#include "HeightMapTexture.h"
#include "YAMLOverloads.h"

#include <SOIL2.h>
#include <iostream>

GLuint HeightMapTexture::useHeightMapUniform = 0;
GLuint HeightMapTexture::heightMapTextureUniform = 0;
GLuint HeightMapTexture::heightMapScaleUniform = 0;
GLuint HeightMapTexture::heightMapOffsetUniform = 0;

HeightMapTexture::HeightMapTexture(const std::string& path, TextureFilterType filterType, TextureWrapType wrapType, float scale)
	: Texture(filterType, wrapType, TextureType::Heightmap), path(path), scale(scale), offset(0.0f)
{
	int width, height;
	uint8_t* data = SOIL_load_image(path.c_str(), &width, &height, 0, SOIL_LOAD_RGB);

	if (data)
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &this->ID);
		glBindTexture(GL_TEXTURE_2D, this->ID);

		// Filtering parameters (We use linear whichif a UV coord doesn't correspond to to a color value in the texture, it will take the average of colors from its neighbours)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterType == TextureFilterType::Linear ?  GL_LINEAR : GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterType == TextureFilterType::Linear ? GL_LINEAR : GL_NEAREST);

		// Wrapping paramters
		GLenum wrap = wrapType == TextureWrapType::Repeat ? GL_REPEAT : GL_CLAMP;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, wrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data); // Tell OpenGL about our image (passes in the width, height and image data (pixels))

		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else
	{
		std::cout << "Failed to load texture '" << path << "'." << std::endl;
	}
}

HeightMapTexture::~HeightMapTexture()
{
	glDeleteTextures(1, &this->ID);
}

void HeightMapTexture::Bind(uint32_t slot) const
{
	glBindTextureUnit(slot, this->ID);
	glUniform1i(heightMapTextureUniform, slot);
	glUniform1f(heightMapScaleUniform, this->scale);
	glUniform3f(heightMapOffsetUniform, this->offset.x, this->offset.y, this->offset.z);
	glUniform1f(useHeightMapUniform, (GLfloat)GL_TRUE);
}

void HeightMapTexture::UnBind(uint32_t slot) const
{
	glUniform1f(useHeightMapUniform, (GLfloat)GL_FALSE);
	glBindTextureUnit(slot, 0);
}

void HeightMapTexture::InitializeUniforms(Ref<Shader> shader)
{
	HeightMapTexture::useHeightMapUniform = glGetUniformLocation(shader->GetID(), "useHeightMap");
	HeightMapTexture::heightMapTextureUniform = glGetUniformLocation(shader->GetID(), "heightMapTexture");
	HeightMapTexture::heightMapScaleUniform = glGetUniformLocation(shader->GetID(), "heightMapScale");
	HeightMapTexture::heightMapOffsetUniform = glGetUniformLocation(shader->GetID(), "heightMapUVOffsetRotation");
}