#include "Renderer.h"

#include <glm/gtc/type_ptr.hpp> 
#include <sstream>
#include "HeightMapTexture.h"

GLuint Renderer::isOverrideColorUniform = 0;
GLuint Renderer::colorOverrideUniform = 0;

GLuint Renderer::ignoreLightingUniform = 0;
GLuint Renderer::alphaTransparencyUniform = 0;

GLuint Renderer::matViewUniform = 0;
GLuint Renderer::matProjectionUniform = 0;
GLuint Renderer::cameraPositionUniform = 0;

std::vector<GLuint> Renderer::textureRatioScales;
GLuint Renderer::alphaTextureScaleUniform = 0;

GLFWwindow* Renderer::window = NULL;

void Renderer::Initialize(const Ref<Shader> shader)
{
	shader->Bind();

	Renderer::isOverrideColorUniform = glGetUniformLocation(shader->GetID(), "isOverrideColor");
	Renderer::colorOverrideUniform = glGetUniformLocation(shader->GetID(), "colorOverride");

	Renderer::ignoreLightingUniform = glGetUniformLocation(shader->GetID(), "isIgnoreLighting");
	Renderer::alphaTransparencyUniform = glGetUniformLocation(shader->GetID(), "alphaTransparency");

	Renderer::matViewUniform = glGetUniformLocation(shader->GetID(), "matView");
	Renderer::matProjectionUniform = glGetUniformLocation(shader->GetID(), "matProjection");
	Renderer::cameraPositionUniform = glGetUniformLocation(shader->GetID(), "cameraPosition");

	Renderer::textureRatioScales.resize(8);
	for (int i = 0; i < 8; i++)
	{
		std::stringstream ss;
		ss << "textureRatioScale" << i;
		Renderer::textureRatioScales[i] = glGetUniformLocation(shader->GetID(), ss.str().c_str());
	}

	Renderer::alphaTextureScaleUniform = glGetUniformLocation(shader->GetID(), "aTexScale");

	Renderer::window = glfwGetCurrentContext();
}

void Renderer::BeginFrame(Ref<Shader> shader, Ref<Camera> camera)
{
	int width, height;
	glfwGetFramebufferSize(window, &width, &height); // Assign width and height to our window width and height
	float ratio = width / (float)height;

	glViewport(0, 0, width, height); // Specifies the transformation of device coords to window coords 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clears the buffers

	shader->Bind();
	glUniformMatrix4fv(matViewUniform, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix())); // Assign new view matrix
	glUniformMatrix4fv(matProjectionUniform, 1, GL_FALSE, glm::value_ptr(glm::perspective(0.6f, ratio, 0.5f, 10000.0f))); // Assign projection
	glUniform4f(cameraPositionUniform, camera->position.x, camera->position.y, camera->position.z, 1.0f);
}

void Renderer::RenderMeshWithTextures(Ref<Shader> shader, Ref<Mesh> mesh, const std::vector<Ref<SceneTextureData>>& textures, const glm::mat4& transform, float alphaTransparency, bool debugMode)
{
	shader->Bind();
	shader->SetMat4x4("matModel", transform);
	shader->SetMat4x4("matModelInverseTranspose", glm::inverse(transform));

	// Bind textures
	int diffuseTextureindex = 0;
	for (int i = 0; i < textures.size(); i++)
	{
		const Ref<SceneTextureData>& textureData = textures[i];
		TextureType textureType = textureData->texture->GetType();
		if (textureType == TextureType::Diffuse)
		{
			glUniform2f(Renderer::textureRatioScales[diffuseTextureindex], textureData->ratio, textureData->texCoordScale); // Setup texture ratio & scales
			textureData->texture->Bind(diffuseTextureindex);
			diffuseTextureindex++;
		}
		else if (textureType == TextureType::Heightmap)
		{
			textureData->texture->Bind(37);
		}
		else if (textureType == TextureType::Discard)
		{
			textureData->texture->Bind(20);
		}
		else if (textureType == TextureType::Alpha)
		{
			glUniform1f(Renderer::alphaTextureScaleUniform, textureData->texCoordScale);
			textureData->texture->Bind(21);
		}
	}

	for (int i = diffuseTextureindex; i < 8; i++)
	{
		glUniform2f(Renderer::textureRatioScales[i], 0.0f, 1.0f); // Make sure we set unused diffuse texture ratios to 0
	}

	glUniform1f(alphaTransparencyUniform, alphaTransparency);

	if (debugMode)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		mesh->GetBoundingBox().Draw(glm::vec3(transform[3]), glm::vec3(1.0f, 1.0f, 1.0f)); // TODO: Retreive scale from transform mat
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	mesh->GetVertexArray()->Bind();
	glDrawElements(GL_TRIANGLES, mesh->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, 0); // TODO: Make an instanced rendering version of this
	mesh->GetVertexArray()->Unbind();

	// Unbind textures
	for (int i = 0; i < textures.size(); i++)
	{
		uint32_t slot = textures[i]->texture->GetType() == TextureType::Heightmap ? 37 
			: textures[i]->texture->GetType() == TextureType::Discard ? 20 
			: textures[i]->texture->GetType() == TextureType::Alpha ? 21 : i;
		textures[i]->texture->UnBind(slot);
	}
}

void Renderer::EndFrame()
{
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void Renderer::RenderMeshWithColorOverride(Ref<Shader> shader, Ref<Mesh> mesh, const glm::mat4& transform, const glm::vec3& colorOverride, bool debugMode, bool ignoreLight)
{
	shader->Bind();
	shader->SetMat4x4("matModel", transform);
	shader->SetMat4x4("matModelInverseTranspose", glm::inverse(transform));

	glUniform1f(isOverrideColorUniform, (float)GL_TRUE);
	glUniform4f(colorOverrideUniform, colorOverride.x, colorOverride.y, colorOverride.z, 1.0f);

	if (ignoreLight)
	{
		glUniform1f(ignoreLightingUniform, (float)GL_TRUE);
	}

	if (debugMode)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		mesh->GetBoundingBox().Draw(glm::vec3(transform[3]), glm::vec3(1.0f, 1.0f, 1.0f)); // TODO: Retreive scale from transform mat
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	mesh->GetVertexArray()->Bind();
	glDrawElements(GL_TRIANGLES, mesh->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, 0); // TODO: Make an instanced rendering version of this
	mesh->GetVertexArray()->Unbind();

	glUniform1f(isOverrideColorUniform, (float)GL_FALSE);

	if (ignoreLight)
	{
		glUniform1f(ignoreLightingUniform, (float)GL_FALSE);
	}
}