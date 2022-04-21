#include "AlphaTexture.h"

#include <SOIL2.h>

GLuint AlphaTexture::isAlphaTextureUniform = 0;
GLuint AlphaTexture::alphaTextureUniform = 0;

AlphaTexture::AlphaTexture(const std::string& path, TextureFilterType filterType, TextureWrapType wrapType, bool genMipMaps)
	: Texture(filterType, wrapType, TextureType::Alpha), path(path), genMipMaps(genMipMaps)
{
	std::cout << "Loading alpha texture..." << std::endl;

	int width, height;
	uint8_t* data = SOIL_load_image(path.c_str(), &width, &height, 0, SOIL_LOAD_RGB);

	if (data)
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &this->ID);
		glBindTexture(GL_TEXTURE_2D, this->ID);

		// Filtering parameters (We use linear whichif a UV coord doesn't correspond to to a color value in the texture, it will take the average of colors from its neighbours)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterType == TextureFilterType::Linear ? genMipMaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR : GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterType == TextureFilterType::Linear ? GL_LINEAR : GL_NEAREST);

		// Wrapping paramters
		GLenum wrap = wrapType == TextureWrapType::Repeat ? GL_REPEAT : GL_CLAMP;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, wrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data); // Tell OpenGL about our image (passes in the width, height and image data (pixels))

		if (genMipMaps)
		{
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		glBindTexture(GL_TEXTURE_2D, 0);

		std::cout << "Texture '" << path << "' loaded successfully!" << std::endl;
	}
	else
	{
		std::cout << "Failed to load texture '" << path << "'." << std::endl;
	}
}

AlphaTexture::~AlphaTexture()
{
	glDeleteTextures(1, &this->ID);
}

void AlphaTexture::Bind(uint32_t slot) const
{
	glUniform1f(isAlphaTextureUniform, (GLfloat)GL_TRUE);
	glBindTextureUnit(slot, this->ID);
	glUniform1i(alphaTextureUniform, slot);
}

void AlphaTexture::UnBind(uint32_t slot) const
{
	glUniform1f(isAlphaTextureUniform, (GLfloat)GL_FALSE);
	glBindTextureUnit(slot, 0);
}

void AlphaTexture::InitializeUniforms(Ref<Shader>shader)
{
	AlphaTexture::isAlphaTextureUniform = glGetUniformLocation(shader->GetID(), "isAlphaTexture");
	AlphaTexture::alphaTextureUniform = glGetUniformLocation(shader->GetID(), "forSomeReasonOtherNamesDontWork");
}