#pragma once

#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "EnvironmentMap.h"
#include "SceneTextureData.h"

#include "GLCommon.h"

class Renderer
{
public:
	static void Initialize(const Ref<Shader> shader);

	static void BeginFrame(Ref<Shader> shader, Ref<Camera> camera);
	static void EndFrame();

	static void RenderMeshWithColorOverride(Ref<Shader> shader, Ref<Mesh> mesh, const glm::mat4& transform, const glm::vec3& colorOverride, bool debugMode = false, bool ignoreLight = false);
	static void RenderMeshWithTextures(Ref<Shader> shader, Ref<Mesh> mesh, const std::vector<Ref<SceneTextureData>>& textures, const glm::mat4& transform, float alphaTransparency, bool debugMode = false);

private:
	static GLuint isOverrideColorUniform;
	static GLuint colorOverrideUniform;

	static GLuint ignoreLightingUniform;
	static GLuint alphaTransparencyUniform;

	static GLuint matViewUniform;
	static GLuint matProjectionUniform;
	static GLuint cameraPositionUniform;

	static std::vector<GLuint> textureRatioScales;
	static GLuint alphaTextureScaleUniform;

	static GLFWwindow* window;
};