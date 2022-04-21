#include "Scene.h"
#include "MeshManager.h"
#include "Renderer.h"
#include "YAMLOverloads.h"

#include <sstream>
#include <fstream>

static const float MaxSpreadRadius = 5500.0f;
const std::vector<UUID> atmosphereLights = { 129824329036021396, 129824329036021395 };
const std::vector<UUID> spotLights = { 129824329036021381, 129824329036021382, 129824329036021383 };
const std::vector<UUID> lightShafts = { 9671491309700141238, 2066726669122141605, 9749425813235041119 };
const float nightLightMoveAngle = 0.1f;
const int nightLightMoveIterations = 300;

Scene::Scene(Ref<Shader> shader)
	: shader(shader), 
	transparentEnd(-1), 
	debugMode(false), 
	scenePanel(this), 
	currentMeshIndex(0), 
	currentLightIndex(0), 
	showCurrentEdit(true),
	camera(NULL),
	mossRadius(0.0f),
	vineRadius(0.0f),
	vineHeight(0.0f),
	vineTexture(nullptr),
	mossTexture(nullptr),
	lightMesh(nullptr),
	night(false),
	dimLights(false),
	startMossSpread(false),
	lightMoveIterations(0)
{
	{
		std::stringstream ss;
		ss << SOLUTION_DIR << "Extern\\assets\\models\\ISO_Sphere.ply";
		this->lightMesh = MeshManager::LoadMesh(ss.str());
	}

	{
		std::stringstream ss;
		ss << SOLUTION_DIR << "Extern\\assets\\textures\\moss.jpg";
		mossTexture = CreateRef<DiffuseTexture>(ss.str(), TextureFilterType::Linear, TextureWrapType::Repeat);
	}
	{
		std::stringstream ss;
		ss << SOLUTION_DIR << "Extern\\assets\\textures\\vines.jpg";
		vineTexture = CreateRef<DiffuseTexture>(ss.str(), TextureFilterType::Linear, TextureWrapType::Repeat);
	}
}

void Scene::Save(const std::string& path)
{
	if (path.empty())
	{
		return;
	}

	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "Scene" << YAML::Value << "Untitled";

	if (this->camera)
	{
		out << YAML::Key << "Camera" << YAML::Value;
		out << YAML::BeginMap;
		out << YAML::Key << "Position" << YAML::Value << this->camera->position;
		out << YAML::Key << "Direction" << YAML::Value << this->camera->direction;
		out << YAML::Key << "Yaw" << YAML::Value << this->camera->yaw;
		out << YAML::Key << "Pitch" << YAML::Value << this->camera->pitch;
		out << YAML::EndMap;
	}

	// Save EnvMap
	if (this->envMap)
	{
		out << YAML::Key << "EnvironmentMap" << YAML::Value;
		this->envMap->Save(out);
	}

	// Save lights
	out << YAML::Key << "Lights" << YAML::Value << YAML::BeginSeq;
	{
		std::unordered_map<UUID, Ref<SceneLight>>::iterator it = lights.begin();
		while(it != lights.end())
		{

			it->second->Save(out);
			it++;
		}
	}
	out << YAML::EndSeq;



	// Save meshes
	out << YAML::Key << "Meshes" << YAML::Value << YAML::BeginSeq;
	std::unordered_map<UUID, Ref<SceneMeshData>>::iterator meshIt;
	for (meshIt = this->meshes.begin(); meshIt != this->meshes.end(); meshIt++)
	{
		meshIt->second->Save(out);
	}
	out << YAML::EndSeq;

	out << YAML::EndMap;

	std::ofstream ofs(path);
	ofs << out.c_str();
}

void Scene::Load(const std::string& path)
{
	std::ifstream ifs(path);
	if (!ifs.good())
	{
		std::cout << "Scene does not exist!" << std::endl;
		return;
	}

	this->meshes.clear();
	this->sortedMeshes.clear();
	this->lights.clear();
	this->transparentEnd = -1;
	this->currentMeshIndex = 0;
	this->scenePanel.SetMeshData(NULL);

	std::stringstream ss;
	ss << ifs.rdbuf();

	shader->Bind();
	
	YAML::Node root = YAML::Load(ss.str());
	if (!root["Scene"])
	{
		std::cout << "Error loading scene file " << path << std::endl;
		return;
	}

	std::string sceneName = root["Scene"].as<std::string>();

	const YAML::Node& cameraNode = root["Camera"];
	if (cameraNode && this->camera)
	{
		this->camera->position = cameraNode["Position"].as<glm::vec3>();
		this->camera->direction = cameraNode["Direction"].as<glm::vec3>();
		this->camera->yaw = cameraNode["Yaw"].as<float>();
		this->camera->pitch = cameraNode["Pitch"].as<float>();
	}

	// Load Env map
	const YAML::Node& envMapNode = root["EnvironmentMap"];
	if (envMapNode)
	{		
		this->envMap = EnvironmentMap::StaticLoad(envMapNode);
	}

	// Load Lights
	const YAML::Node& lightNode = root["Lights"];
	if (lightNode)
	{
		YAML::const_iterator it;
		for (it = lightNode.begin(); it != lightNode.end(); it++)
		{
			YAML::Node childNode = (*it);
			Ref<SceneLight> light = SceneLight::StaticLoad(childNode);

			light->light->SendToShader();
			this->lights.insert({ light->uuid, light });
			lightVec.push_back(light);
		}
	}

	const YAML::Node& meshNode = root["Meshes"];
	if (meshNode)
	{
		YAML::const_iterator it;
		for (it = meshNode.begin(); it != meshNode.end(); it++)
		{
			const YAML::Node& node = (*it);
			Ref<SceneMeshData> meshData = SceneMeshData::StaticLoad(node);
			AddMesh(meshData);
		}
	}
}

void Scene::AddLight(const glm::vec3& position)
{
	int lightIndex = this->lights.size();
	if (lightIndex >= MAX_LIGHTS)
	{
		std::cout << "The maximum number of lights has been reached. You must increase the MAX_LIGHTS value in LightManager.h AND the fragment shader to inccrease the number of lights." << std::endl;
		return;
	}

	Ref<Light> light = CreateRef<Light>(lightIndex);
	light->position = glm::vec4(position, 1.0f);
	Ref<SceneLight> sLight = CreateRef<SceneLight>(light);

	shader->Bind();
	light->SendToShader();
	this->lights.insert({ sLight->uuid, sLight });
	lightVec.push_back(sLight);
}

void Scene::AddLight(Ref<SceneLight> light)
{
	shader->Bind();
	light->light->SendToShader();
	this->lights.insert({ light->uuid, light });
	lightVec.push_back(light);
}

void Scene::AddMesh(Ref<SceneMeshData> meshData)
{
	this->meshes.insert(std::make_pair(meshData->uuid, meshData) );

	if (!this->sortedMeshes.empty() && meshData->alphaTransparency < 1.0f) // We have to add transparent objects to the front so we can handle blend transparency
	{
		this->sortedMeshes.insert(this->sortedMeshes.begin(), meshData);
		transparentEnd++;
	}
	else
	{
		this->sortedMeshes.push_back(meshData);
	}
}

void Scene::OnUpdate(Ref<Camera> camera, float deltaTime)
{
	if (startMossSpread) // Spread the moss over the scene
	{
		if (mossRadius >= MaxSpreadRadius && vineRadius < MaxSpreadRadius)
		{
			vineHeight += 5.0f * deltaTime;
			vineRadius += 50.0f * deltaTime;
		}
		else // Expand moss
		{
			mossRadius += 500.0f * deltaTime;
		}

		shader->SetFloat3("spreadData", glm::vec3(mossRadius, vineRadius, vineHeight));
	}

	if (night)
	{
		if (!dimLights)
		{
			for (const UUID& id : spotLights)
			{
				const glm::vec4& direction = lights.at(id)->light->direction;
				lights.at(id)->light->EditDirection(direction.x, direction.y, direction.z - nightLightMoveAngle * deltaTime, direction.w);
			}

			for (const UUID& id : lightShafts)
			{
				meshes.at(id)->orientation.x += nightLightMoveAngle * deltaTime;
			}

			// TODO: Move light beams

			lightMoveIterations++;
			if (lightMoveIterations >= nightLightMoveIterations)
			{
				dimLights = true;
			}
		}
		else
		{
			// Dim the lights
			for (const UUID& id : spotLights)
			{
				const glm::vec4& atten = lights.at(id)->light->attenuation;
				lights.at(id)->light->EditAttenuation(atten.x, atten.y + 0.01f * deltaTime, atten.z, atten.w);
			}

			for (const UUID& id : atmosphereLights)
			{
				const glm::vec4& atten = lights.at(id)->light->attenuation;
				lights.at(id)->light->EditAttenuation(atten.x, atten.y + 0.001f * deltaTime, atten.z, atten.w);
			}

			for (const UUID& id : lightShafts)
			{
				meshes.at(id)->alphaTransparency -= 0.8f * deltaTime;
			}
		}
	}

	glDisable(GL_DEPTH);
	glDisable(GL_DEPTH_TEST);

	// Draw environment map
	if (envMap)
	{
		this->envMap->Draw(this->shader, camera->position, glm::vec3(10.0f, 10.0f, 10.0f));
	}

	// Draw meshes
	glEnable(GL_DEPTH);
	glEnable(GL_DEPTH_TEST);

	for (int i = 1; i <= this->transparentEnd; i++) // Sort the transparent meshes (use insertion sort here since they are sorted the majority of the time)
	{
		int j = i - 1;

		Ref<SceneMeshData> cachedMeshData = this->sortedMeshes[i];
		glm::vec3 difference = camera->position - cachedMeshData->position;
		float currentDistance = glm::dot(difference, difference);

		glm::vec3 difference2 = camera->position - this->sortedMeshes[j]->position;
		float toCompare = glm::dot(difference2, difference2);

		while (j >= 0 && toCompare < currentDistance)
		{
			this->sortedMeshes[j + 1] = this->sortedMeshes[j];
			j = j - 1;
		}

		this->sortedMeshes[j + 1] = cachedMeshData;
	}

	glDisable(GL_BLEND);

	// Render opaque meshes
	int startIndex = this->transparentEnd == -1 ? 0 : this->transparentEnd + 1;
	std::vector<int> changedAlphaValues;
	for (int i = startIndex; i < this->sortedMeshes.size(); i++)
	{
		Ref<SceneMeshData> meshData = this->sortedMeshes[i];
		glm::mat4 transform(1.0f);
		transform *= glm::translate(glm::mat4(1.0f), meshData->position);
		transform *= glm::rotate(glm::mat4(1.0f), meshData->orientation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		transform *= glm::rotate(glm::mat4(1.0f), meshData->orientation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		transform *= glm::rotate(glm::mat4(1.0f), meshData->orientation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		transform *= glm::scale(glm::mat4(1.0f), meshData->scale);

		if (meshData->alphaTransparency < 1.0f || meshData->hasAlphaTransparentTexture) // Alpha transparency is now below 1, we need to flag this object to be sorted
		{
			changedAlphaValues.push_back(i);
		}

		if (meshData == this->scenePanel.GetEditMesh() && this->showCurrentEdit)
		{
			Renderer::RenderMeshWithColorOverride(shader, meshData->mesh, transform, glm::vec3(0.0f, 1.0f, 0.0f), this->debugMode, true);
		}
		else
		{
			Renderer::RenderMeshWithTextures(shader, meshData->mesh, meshData->textures, transform, meshData->alphaTransparency, this->debugMode);
		}
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (int i = 0; i <= this->transparentEnd; i++)
	{
		Ref<SceneMeshData> meshData = this->sortedMeshes[i];
		glm::mat4 transform(1.0f);
		transform *= glm::translate(glm::mat4(1.0f), meshData->position);
		transform *= glm::rotate(glm::mat4(1.0f), meshData->orientation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		transform *= glm::rotate(glm::mat4(1.0f), meshData->orientation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		transform *= glm::rotate(glm::mat4(1.0f), meshData->orientation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		transform *= glm::scale(glm::mat4(1.0f), meshData->scale);

		if (meshData == this->scenePanel.GetEditMesh() && this->showCurrentEdit)
		{
			Renderer::RenderMeshWithColorOverride(shader, meshData->mesh, transform, glm::vec3(0.0f, 1.0f, 0.0f), this->debugMode, true);
		}
		else
		{
			Renderer::RenderMeshWithTextures(shader, meshData->mesh, meshData->textures, transform, meshData->alphaTransparency, this->debugMode);
		}
	}

	glDisable(GL_BLEND);

	if (!changedAlphaValues.empty())
	{
		for (int& i : changedAlphaValues) // Ad changed alpha meshes to the front of the vector
		{
			std::rotate(this->sortedMeshes.begin(), this->sortedMeshes.begin() + i, this->sortedMeshes.begin() + (i + 1));
			//std::rotate(this->sortedMeshes.rend() - i, this->sortedMeshes.rend() - i, this->sortedMeshes.rend() - 0);
		}
		this->transparentEnd += changedAlphaValues.size(); // Make sure we update our transparent end index
	}

	std::unordered_map<UUID, Ref<SceneLight>>::iterator it = lights.begin();
	while (it != lights.end())
	{
		Ref<SceneLight> light = it->second;

		for (Ref<LightAttachment> attch : light->attachements)
		{
			attch->OnUpdate(deltaTime);
		}

		// Draw lights
		if (debugMode)
		{
			glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(light->light->position));
			{
				float distTo95Percent = Light::CalcApproxDistFromAtten(0.95f, 0.01, 10000.0f, light->light->attenuation.x, light->light->attenuation.y, light->light->attenuation.z, 50);
				glm::mat4 transform(1.0f);
				transform *= translate;
				transform *= glm::scale(glm::mat4(1.0f), glm::vec3(distTo95Percent, distTo95Percent, distTo95Percent));
				Renderer::RenderMeshWithColorOverride(this->shader, this->lightMesh, transform, glm::vec3(1.0f, 0.0f, 0.0f), this->debugMode, true);
			}
			{
				float distTo50Percent = Light::CalcApproxDistFromAtten(0.5f, 0.01, 10000.0f, light->light->attenuation.x, light->light->attenuation.y, light->light->attenuation.z, 50);
				glm::mat4 transform(1.0f);
				transform *= translate;
				transform *= glm::scale(glm::mat4(1.0f), glm::vec3(distTo50Percent, distTo50Percent, distTo50Percent));
				Renderer::RenderMeshWithColorOverride(this->shader, this->lightMesh, transform, glm::vec3(1.0f, 1.0f, 0.0f), this->debugMode, true);
			}
			{
				float distTo25Percent = Light::CalcApproxDistFromAtten(0.25f, 0.01, 10000.0f, light->light->attenuation.x, light->light->attenuation.y, light->light->attenuation.z, 50);
				glm::mat4 transform(1.0f);
				transform *= translate;
				transform *= glm::scale(glm::mat4(1.0f), glm::vec3(distTo25Percent, distTo25Percent, distTo25Percent));
				Renderer::RenderMeshWithColorOverride(this->shader, this->lightMesh, transform, glm::vec3(0.0f, 1.0f, 0.0f), this->debugMode, true);
			}
			{
				float distTo5Percent = Light::CalcApproxDistFromAtten(0.25f, 0.01, 10000.0f, light->light->attenuation.x, light->light->attenuation.y, light->light->attenuation.z, 50);
				glm::mat4 transform(1.0f);
				transform *= translate;
				transform *= glm::scale(glm::mat4(1.0f), glm::vec3(distTo5Percent, distTo5Percent, distTo5Percent));
				Renderer::RenderMeshWithColorOverride(this->shader, this->lightMesh, transform, glm::vec3(0.0f, 0.0f, 1.0f), this->debugMode, true);
			}
			glm::mat4 transform(1.0f);
			transform *= translate;
			Renderer::RenderMeshWithColorOverride(this->shader, this->lightMesh, transform, glm::vec3(1.0f, 1.0f, 1.0f), this->debugMode, true);
		}

		it++;
	}

	scenePanel.OnUpdate(deltaTime);
}

void Scene::NextMesh()
{
	if (this->sortedMeshes.empty())
	{
		this->scenePanel.SetMeshData(NULL);
		return;
	}

	this->currentMeshIndex = std::min(this->currentMeshIndex + 1, (int)(this->sortedMeshes.size() - 1));
	this->scenePanel.SetMeshData(sortedMeshes[currentMeshIndex]);
}

void Scene::PreviousMesh()
{
	if (this->sortedMeshes.empty())
	{
		this->scenePanel.SetMeshData(NULL);
		return;
	}

	this->currentMeshIndex = std::max(this->currentMeshIndex - 1, 0);
	this->scenePanel.SetMeshData(sortedMeshes[currentMeshIndex]);
}

void Scene::LastMesh()
{
	if (this->sortedMeshes.empty())
	{
		this->scenePanel.SetMeshData(NULL);
		return;
	}

	this->currentMeshIndex = this->sortedMeshes.size() - 1;
}

void Scene::FirstMesh()
{
	if (this->sortedMeshes.empty())
	{
		this->scenePanel.SetMeshData(NULL);
		return;
	}

	this->currentMeshIndex = 0;
}

void Scene::NextLight()
{
	if (this->lightVec.empty())
	{
		this->scenePanel.SetLight(NULL);
		return;
	}

	this->currentLightIndex = std::min(this->currentLightIndex + 1, (int)(this->lightVec.size() - 1));
	this->scenePanel.SetLight(lightVec[currentLightIndex]);
}

void Scene::PreviousLight()
{
	if (this->lightVec.empty())
	{
		this->scenePanel.SetLight(NULL);
		return;
	}

	this->currentLightIndex = std::max(this->currentLightIndex - 1, 0);
	this->scenePanel.SetLight(lightVec[currentLightIndex]);
}

void Scene::StartMossSpread()
{
	if (!startMossSpread)
	{
		startMossSpread = true;
		mossTexture->Bind(6);
		vineTexture->Bind(7);
		shader->SetFloat3("mossSpreadStartPos", glm::vec3(3125.0f, 0.0f, 3125.0f));
	}
}

void Scene::StartNightCycle()
{
	night = true;
}