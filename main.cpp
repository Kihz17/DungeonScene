#pragma once

#include "GLCommon.h"

#include <glm/glm.hpp>
#include <glm/vec3.hpp> 
#include <glm/vec4.hpp> 
#include <glm/mat4x4.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp> 

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector> 
#include <fstream>
#include <sstream>
#include <unordered_set>

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_opengl3.h"
#include "vendor/imgui/imgui_impl_glfw.h"

#include "Camera.h"
#include "Light.h"
#include "EnvironmentMap.h"
#include "AABB.h"
#include "Raycast.h"
#include "Scene.h"
#include "Renderer.h"
#include "MeshManager.h"
#include "TextureManager.h"
#include "FlickerAttachment.h"

const float windowWidth = 1700;
const float windowHeight = 800;
bool editMode = true;

Ref<Camera> camera = CreateRef<Camera>(windowHeight, windowWidth);
float moveSpeed = 40.1f;

Ref<Scene> scene = NULL;

int startIndex = 18;

static float getRandom(float low, float high)
{
	return low + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX) / (high - low));
}

static int GetRandomInt(int low, int high)
{
	return low + static_cast<int>(rand()) / (static_cast<int>(RAND_MAX) / (high - low));
}

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Movement controls
	if (!editMode)
	{
		if (key == GLFW_KEY_W)
		{
			camera->position += camera->direction * moveSpeed;
		}
		if (key == GLFW_KEY_A)
		{
			// Rotate our camera's direction 90 degrees to the left
			glm::vec3 left;
			constexpr float theta = glm::radians(90.0f);
			left.x = camera->direction.x * cos(theta) + camera->direction.z * sin(theta);
			left.y = 0.0f;
			left.z = -camera->direction.x * sin(theta) + camera->direction.z * cos(theta);
			camera->position += left * moveSpeed;
		}
		if (key == GLFW_KEY_S)
		{
			camera->position -= camera->direction * moveSpeed;
		}
		if (key == GLFW_KEY_D)
		{
			// Rotate our camera's direction 90 degrees to the right
			glm::vec3 right;
			constexpr float theta = glm::radians(-90.0f);
			right.x = camera->direction.x * cos(theta) + camera->direction.z * sin(theta);
			right.y = 0.0f;
			right.z = -camera->direction.x * sin(theta) + camera->direction.z * cos(theta);
			camera->position += right * moveSpeed;
		}
		if (key == GLFW_KEY_SPACE)
		{
			camera->position.y += moveSpeed;
		}
	}

	if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
	{
		scene->debugMode = !scene->debugMode;
	}

	if (key == GLFW_KEY_PAGE_UP && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		scene->NextMesh();
	}
	else if (key == GLFW_KEY_PAGE_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		scene->PreviousMesh();
	}
	else if (key == GLFW_KEY_EQUAL && action == GLFW_PRESS)
	{
		scene->LastMesh();
	}
	else if (key == GLFW_KEY_MINUS && action == GLFW_PRESS)
	{
		scene->FirstMesh();
	}

	if (key == GLFW_KEY_APOSTROPHE && action == GLFW_PRESS)
	{
		scene->StartNightCycle();
	}

	if (key == GLFW_KEY_BACKSLASH && action == GLFW_PRESS)
	{
		scene->StartMossSpread();
	}

	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
	{
		scene->NextLight();
	}
	else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
	{
		scene->PreviousLight();
	}

	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS)
	{
		scene->showCurrentEdit = !scene->showCurrentEdit;
	}

	if (key == GLFW_KEY_RIGHT_SHIFT && action == GLFW_PRESS)
	{
		Ref<SceneLight> light = CreateRef<SceneLight>(CreateRef<Light>(startIndex++));
		light->light->position = glm::vec4(camera->position, 1.0f);
		light->light->diffuse = glm::vec4(1.0f, 0.9f, 0.0f, 1.0f);
		light->light->specular = glm::vec4(1.0f, 0.9f, 0.0f, 1.0f);
		light->light->attenuation = glm::vec4(0.0f, 0.10900525f, 0.0f, 100000.0f);
		light->attachements.push_back(CreateRef<FlickerAttachment>(light->light));
		scene->AddLight(light);
	}

	// Toggle cursor view
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		editMode = !editMode;
		int cursorOption = editMode ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED;
		glfwSetInputMode(window, GLFW_CURSOR, cursorOption);
		if (!editMode) // We switched to edit mode, set our last cursor pos
		{
			glfwSetCursorPos(glfwGetCurrentContext(), camera->lastWindowX, camera->lastWindowY);
		}
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (!editMode)
	{
		camera->MoveCamera(xpos, ypos);
	}
}

static void drop_callback(GLFWwindow* window, int count, const char** paths)
{
	std::stringstream ss;
	ss << SOLUTION_DIR << "Extern\\assets\\models\\";
	std::string path(paths[0]);
	if (path.find(ss.str()) == std::string::npos)
	{
		std::cout << "Model import must be in the models directory!" << std::endl;
		return;
	}

	Ref<Mesh> mesh = MeshManager::LoadMesh(path);
	Ref<SceneMeshData> meshData = CreateRef<SceneMeshData>(mesh);
	meshData->position = camera->position + (camera->direction * 10.0f);
	scene->AddMesh(meshData);
}

void LoadFile(const std::string& file, Ref<Scene> scene);
void ParseDungeon(const std::string& file, Ref<Scene> scene);
void ParseDoors(const std::string& file, Ref<Scene> scene);

int main(void)
{
	GLFWwindow* window;

	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
	{
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	// Initialize our window
	window = glfwCreateWindow(windowWidth, windowHeight, "Midterm", NULL, NULL);

	// Make sure the window initialized properly
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback); // Tell GLFW where our key callbacks are
	glfwSetCursorPosCallback(window, mouse_callback); // Tell GLFW where our mouse callback is
	glfwSetDropCallback(window, drop_callback);

	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc) glfwGetProcAddress); // Give glad this process ID
	glfwSwapInterval(1);

	// Initialize ImGui
	ImGui::CreateContext(); 
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 420");
	ImGui::StyleColorsDark();

	// Load shader
	std::stringstream ss;
	ss << SOLUTION_DIR << "Extern\\assets\\shaders\\vertexShader.glsl";
	std::string vertexPath = ss.str();
	ss.str("");

	ss << SOLUTION_DIR << "Extern\\assets\\shaders\\fragmentShader.glsl";
	std::string fragmentPath = ss.str();
	ss.str("");

	Ref<Shader> shader = CreateRef<Shader>("Shader#1", vertexPath, fragmentPath);

	Renderer::Initialize(shader);
	Light::InitializeUniforms(shader);
	DiffuseTexture::InitializeUniforms(shader);
	HeightMapTexture::InitializeUniforms(shader);
	DiscardTexture::InitializeUniforms(shader);
	AlphaTexture::InitializeUniforms(shader);

	scene = CreateRef<Scene>(shader);
	scene->camera = camera;

	scene->Load("scene.yaml");

	//ss << SOLUTION_DIR << "dungeon.tsv";
	//ParseDungeon(ss.str(), scene);
	//ParseDoors(ss.str(), scene);

	float fpsFrameCount = 0.f;
	float fpsTimeElapsed = 0.f;
	float previousTime = static_cast<float>(glfwGetTime());

	// Our actual render loop
	while (!glfwWindowShouldClose(window))
	{
		float currentTime = static_cast<float>(glfwGetTime());
		float deltaTime = currentTime - previousTime;
		previousTime = currentTime;

		// FPS TITLE
		{
			fpsTimeElapsed += deltaTime;
			fpsFrameCount += 1.0f;
			if (fpsTimeElapsed >= 0.03f)
			{
				std::string fps = std::to_string(fpsFrameCount / fpsTimeElapsed);
				std::string ms = std::to_string(1000.f * fpsTimeElapsed / fpsFrameCount);
				std::string newTitle = "FPS: " + fps + "   MS: " + ms;
				glfwSetWindowTitle(window, newTitle.c_str());

	
				fpsTimeElapsed = 0.f;
				fpsFrameCount = 0.f;
			}
		}

		Renderer::BeginFrame(shader, camera);

		// Start imGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Safety, mostly for first frame
		if (deltaTime == 0.0f)
		{
			deltaTime = 0.03f;
		}

		scene->OnUpdate(camera, deltaTime);

		// Render imGui
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		Renderer::EndFrame();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window); // Clean up the window

	glfwTerminate(); 
	exit(EXIT_SUCCESS);
}

void LoadFile(const std::string& file, Ref<Scene> scene)
{
	std::ifstream ifs(file);
	if (!ifs.good())
	{
		std::cout << "Could not read dungeon file!" << std::endl;
		return;
	}

	// 1 = Left Wall
	// 2 = Down Wall
	// 3 = Right Wall
	// 4 = Up Wall

	float scale = 0.5f;
	const float wallOffset = 500.0f * scale;
	glm::vec3 scaleVec(scale, scale, scale);

	const std::string wallPaths[6] =
	{
		"SM_Env_Dwarf_Wall_01.ply",
		"SM_Env_Dwarf_Wall_02.ply",
		"SM_Env_Dwarf_Wall_03.ply",
		"SM_Env_Dwarf_Wall_04.ply",
		"SM_Env_Dwarf_Wall_05.ply",
		"SM_Env_Dwarf_Wall_06.ply"
	};

	std::string line;
	int height = 0;
	while (std::getline(ifs, line))
	{
		const char* characters = line.c_str();
		for (int width = 0; width < line.size(); width++)
		{
			char c = characters[width];
			bool wall = c == '1' || c == '2' || c == '3' || c == '4';
			if (wall)
			{
				glm::vec3 orientation(0.0f);
				glm::vec3 position(height * wallOffset, 0.0f, width * wallOffset);
				if (c == '1')
				{
					orientation.y = glm::radians(180.0f);
				}
				else if (c == '2')
				{
					orientation.y = glm::radians(270.0f);
					position.x += wallOffset;
					position.z += wallOffset;
				}
				else if (c == '3')
				{
					position.x += wallOffset;
					position.z += wallOffset;
				}
				else if (c == '4')
				{
					orientation.y = glm::radians(90.0f);

				}

				std::stringstream ss;
				ss << SOLUTION_DIR << "Extern\\assets\\textures\\wall.png";
				Ref<SceneTextureData> textureData = CreateRef<SceneTextureData>(TextureManager::LoadDiffuseTexture(ss.str(), TextureFilterType::Linear, TextureWrapType::Repeat));
				ss.str("");

				ss << SOLUTION_DIR << "Extern\\assets\\models\\Walls\\" << wallPaths[GetRandomInt(0, 5)];
				Ref<SceneMeshData> meshData = CreateRef<SceneMeshData>(MeshManager::LoadMesh(ss.str()));
				meshData->alphaTransparency = 1.0f;
				meshData->AddTexture(textureData);
				meshData->position = position;
				meshData->orientation = orientation;
				meshData->scale = scaleVec;
				scene->AddMesh(meshData);
			}

			if (height > 1)
			{
				std::stringstream ss;
				ss << SOLUTION_DIR << "Extern\\assets\\textures\\wall.png";
				Ref<SceneTextureData> textureData = CreateRef<SceneTextureData>(TextureManager::LoadDiffuseTexture(ss.str(), TextureFilterType::Linear, TextureWrapType::Repeat));
				ss.str("");

				ss << SOLUTION_DIR << "Extern\\assets\\models\\Floors\\SM_Env_Dwarf_Floor_06.ply";
				Ref<SceneMeshData> meshData = CreateRef<SceneMeshData>(MeshManager::LoadMesh(ss.str()));
				meshData->alphaTransparency = 1.0f;
				meshData->AddTexture(textureData);
				meshData->position = glm::vec3(height * wallOffset, 0.0f, width * wallOffset);
				meshData->position.z += wallOffset;
				meshData->orientation = glm::vec3(0.0f, 0.0f, 0.0f);
				meshData->scale = scaleVec;
				scene->AddMesh(meshData);
			}
		}

		height++;
	}

}

void ParseDungeon(const std::string& file, Ref<Scene> scene)
{
	std::ifstream ifs(file);
	if (!ifs.good())
	{
		std::cout << "Could not read dungeon file!" << std::endl;
		return;
	}

	const std::string wallPaths[6] =
	{
		"SM_Env_Dwarf_Wall_01.ply",
		"SM_Env_Dwarf_Wall_02.ply",
		"SM_Env_Dwarf_Wall_03.ply",
		"SM_Env_Dwarf_Wall_04.ply",
		"SM_Env_Dwarf_Wall_05.ply",
		"SM_Env_Dwarf_Wall_06.ply"
	};

	auto AddWall = [](Ref<Scene> scene, const std::string& wallFile, const glm::vec3& position, const glm::vec3& orien, const glm::vec3& scale)
	{
		std::stringstream ss;
		ss << SOLUTION_DIR << "Extern\\assets\\textures\\wall.png";
		Ref<SceneTextureData> textureData = CreateRef<SceneTextureData>(TextureManager::LoadDiffuseTexture(ss.str(), TextureFilterType::Linear, TextureWrapType::Repeat));
		ss.str("");

		ss << SOLUTION_DIR << "Extern\\assets\\models\\Walls\\" << wallFile;
		Ref<SceneMeshData> meshData = CreateRef<SceneMeshData>(MeshManager::LoadMesh(ss.str()));
		meshData->alphaTransparency = 1.0f;
		meshData->AddTexture(textureData);
		meshData->position = position;
		meshData->orientation = orien;
		meshData->scale = scale;
		scene->AddMesh(meshData);
	};

	glm::vec3 scale(0.25f, 0.25f, 0.25f);
	float wallOffset = 500.0f * scale.x;

	std::vector<std::string> lines;
	std::string line;
	while (std::getline(ifs, line))
	{
		lines.push_back(line);
	}

	std::unordered_set<std::string> set;

	for (int i = 0; i < lines.size(); i++)
	{
		std::string l = lines[i];
		const char* characters = l.c_str();
		for (int j = 0; j < l.size(); j++)
		{
			char c = characters[j];
			if (c == '-')
			{
				// Check left
				if (j - 1 >= 0)
				{
					char cLeft = characters[j - 1];
					if (cLeft == 'F' || cLeft == 'D')
					{
						std::stringstream ss;
						ss << i << "-" << (j - 1);

						if (set.find(ss.str()) == set.end())
						{
							AddWall(scene, wallPaths[GetRandomInt(0, 5)], glm::vec3(i * wallOffset, 0.0f, j * wallOffset), glm::vec3(0.0f, 0.0f, 0.0f), scale);
							set.insert(ss.str());
						}
					}
				}
				
				// Check right
				if (j + 1 < l.size())
				{
					char cRight = characters[j - 1];
					if (cRight == 'F' || cRight == 'D')
					{
						std::stringstream ss;
						ss << i << "-" << (j - 1);

						if (set.find(ss.str()) == set.end())
						{
							AddWall(scene, wallPaths[GetRandomInt(0, 5)], glm::vec3(i * wallOffset, 0.0f, j * wallOffset), glm::vec3(0.0f, glm::radians(180.0f), 0.0f), scale);
							set.insert(ss.str());
						}
					}
				}
				
				// Check above
				if (i - 1 >= 0)
				{
					std::string lineAbove = lines[i - 1];
					if (j < lineAbove.size())
					{
						char cAbove = lineAbove[j];
						if (cAbove == 'F' || cAbove == 'D')
						{
							std::stringstream ss;
							ss << (i - 1) << "-" << j;

							if (set.find(ss.str()) == set.end())
							{
								AddWall(scene, wallPaths[GetRandomInt(0, 5)], glm::vec3(((i - 1) * wallOffset) + wallOffset, 0.0f, (j * wallOffset) + wallOffset), glm::vec3(0.0f, glm::radians(270.0f), 0.0f), scale);
								set.insert(ss.str());
							}
						}
					}
				}
				
				// Check below
				if (i + 1 < lines.size())
				{
					std::string lineBelow = lines[i + 1];
					if (j < lineBelow.size())
					{
						char cBelow = lineBelow[j];
						if (cBelow == 'F' || cBelow == 'D')
						{
							std::stringstream ss;
							ss << (i + 1) << "-" << j;

							if (set.find(ss.str()) == set.end())
							{
								AddWall(scene, wallPaths[GetRandomInt(0, 5)], glm::vec3(((i + 1) * wallOffset) + wallOffset, 0.0f, (j * wallOffset) + wallOffset), glm::vec3(0.0f, glm::radians(90.0f), 0.0f), scale);

								set.insert(ss.str());
							}
						}
					}
				}
			}

			if (i > 1)
			{
				std::stringstream ss;
				ss << SOLUTION_DIR << "Extern\\assets\\textures\\wall.png";
				Ref<SceneTextureData> textureData = CreateRef<SceneTextureData>(TextureManager::LoadDiffuseTexture(ss.str(), TextureFilterType::Linear, TextureWrapType::Repeat));
				ss.str("");

				ss << SOLUTION_DIR << "Extern\\assets\\models\\Floors\\SM_Env_Dwarf_Floor_06.ply";
				Ref<SceneMeshData> meshData = CreateRef<SceneMeshData>(MeshManager::LoadMesh(ss.str()));
				meshData->alphaTransparency = 1.0f;
				meshData->AddTexture(textureData);
				meshData->position = glm::vec3(i * wallOffset, 0.0f, j * wallOffset);
				meshData->position.z += (wallOffset * 2.0f);
				meshData->orientation = glm::vec3(0.0f, 0.0f, 0.0f);
				meshData->scale = scale;
				scene->AddMesh(meshData);
			}
		}
	}
}

void ParseDoors(const std::string& file, Ref<Scene> scene)
{
	std::ifstream ifs(file);
	if (!ifs.good())
	{
		std::cout << "Could not read dungeon file!" << std::endl;
		return;
	}

	auto AddDoor = [](Ref<Scene> scene, const glm::vec3& position, const glm::vec3& orien, const glm::vec3& scale)
	{
		std::stringstream ss;
		ss << SOLUTION_DIR << "Extern\\assets\\textures\\wall.png";
		Ref<SceneTextureData> textureData = CreateRef<SceneTextureData>(TextureManager::LoadDiffuseTexture(ss.str(), TextureFilterType::Linear, TextureWrapType::Repeat));
		ss.str("");

		ss << SOLUTION_DIR << "Extern\\assets\\models\\Stairs\\SM_Env_Dwarf_Stairs_01.ply";
		Ref<SceneMeshData> meshData = CreateRef<SceneMeshData>(MeshManager::LoadMesh(ss.str()));
		meshData->alphaTransparency = 1.0f;
		//meshData->AddTexture(textureData);
		meshData->position = position;
		meshData->orientation = orien;
		meshData->scale = scale;
		scene->AddMesh(meshData);
	};

	glm::vec3 scale(0.25f, 0.25f, 0.25f);
	float wallOffset = 500.0f * 0.25;

	std::vector<std::string> lines;
	std::string line;
	while (std::getline(ifs, line))
	{
		lines.push_back(line);
	}

	std::unordered_set<std::string> set;

	for (int i = 0; i < lines.size(); i++)
	{
		std::string l = lines[i];
		const char* characters = l.c_str();
		for (int j = 0; j < l.size(); j++)
		{
			char c = characters[j];
			if (c == 'D')
			{
				AddDoor(scene, glm::vec3(i * wallOffset, 0.0f, j * wallOffset), glm::vec3(0.0f, 0.0f, 0.0f), scale);
			}
		}
	}
}