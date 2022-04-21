#include "ScenePanel.h"
#include "Scene.h"
#include "FlickerAttachment.h"
#include "TextureManager.h"

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_stdlib.h"

const char* ScenePanel::textureTypes[] = { "Diffuse", "Heightmap", "Discard", "Alpha"};
const char* ScenePanel::filterTypes[] = { "Linear", "Nearest" };
const char* ScenePanel::wrapTypes[] = { "Clamp", "Repeat" };

ScenePanel::ScenePanel(Scene* scene) :
	scene(scene), meshData(NULL), selectedTexture(0), selectedFilter(0), selectedWrap(0), genMipMaps(false)
{
}

ScenePanel::~ScenePanel()
{

}

void ScenePanel::OnUpdate(float deltaTime)
{
	ImGui::Begin("Mesh Editor");

	if (this->meshData)
	{
		std::string file = this->meshData->mesh->GetPath();
		file = file.substr(file.find_last_of('\\') + 1);
		ImGui::Text(file.c_str());

		ImGui::NewLine();
		ImGui::DragFloat3("Position", (float*)&this->meshData->position);

		ImGui::NewLine();
		ImGui::DragFloat3("Orientation", (float*)&this->meshData->orientation, 0.01f);

		ImGui::NewLine();
		ImGui::DragFloat3("Scale", (float*)&this->meshData->scale, 0.01f);

		ImGui::NewLine();
		ImGui::DragFloat("Alpha Transparency", (float*) &this->meshData->alphaTransparency, 0.01f, 0.0f, 1.0f);

		ImGui::NewLine();
		if(ImGui::Button("Duplicate"))
		{
			Ref<SceneMeshData> meshData = CreateRef<SceneMeshData>(this->meshData->mesh);
			meshData->position = this->meshData->position;
			meshData->orientation = this->meshData->orientation;
			meshData->scale = this->meshData->scale;
			meshData->alphaTransparency = this->meshData->alphaTransparency;
			meshData->hasAlphaTransparentTexture = this->meshData->hasAlphaTransparentTexture;
			for(const Ref<SceneTextureData>& textureData : this->meshData->textures)
			{
				Ref<SceneTextureData> duplicatedData = CreateRef<SceneTextureData>(textureData->texture, textureData->ratio, textureData->texCoordScale);
				meshData->textures.push_back(duplicatedData);
			}

			this->scene->AddMesh(meshData);
		}
		ImGui::BeginChild("Textures");
		ImGui::NewLine();

		int removeIndex = -1;
		int textureIndex = 0;
		for (const Ref<SceneTextureData>& textureData : this->meshData->textures)
		{
			std::string textureFile = textureData->texture->GetPath();
			textureFile = std::string("Texture: " + textureFile.substr(textureFile.find_last_of("\\") + 1));
			ImGui::Text(textureFile.c_str());
			{
				std::stringstream ss;
				ss << "Texture Ratio " << textureIndex;
				ImGui::DragFloat(ss.str().c_str(), &textureData->ratio, 0.01f);
			}
			{
				std::stringstream ss;
				ss << "Texture Coord Scale " << textureIndex;
				ImGui::DragFloat(ss.str().c_str(), &textureData->texCoordScale, 0.01f);
			}
			{
				std::stringstream ss;
				ss << "Remove Texture " << textureIndex;
				if (ImGui::Button(ss.str().c_str()))
				{
					removeIndex = textureIndex;
				}
			}

			ImGui::NewLine();
			textureIndex++;
		}

		if (removeIndex != -1)
		{
			this->meshData->textures.erase(this->meshData->textures.begin() + removeIndex);
		}
	
		ImGui::Text("Add Texture");
		ImGui::InputText("Texture", &this->textureName);
		ImGui::Combo("Texture Type", &this->selectedTexture, textureTypes, IM_ARRAYSIZE(textureTypes));
		ImGui::Combo("Filter Type", &this->selectedFilter, filterTypes, IM_ARRAYSIZE(filterTypes));
		ImGui::Combo("Wrap Type", &this->selectedWrap, wrapTypes, IM_ARRAYSIZE(wrapTypes));
		ImGui::Checkbox("Gen Mip Maps", &this->genMipMaps);

		if (ImGui::Button("Add Texture"))
		{
			std::stringstream ss;
			ss << SOLUTION_DIR << "Extern\\assets\\textures\\" << this->textureName;
			const char * textureTypeString = textureTypes[this->selectedTexture];
			TextureFilterType filterType = StringToFilterType(filterTypes[this->selectedFilter]);
			TextureWrapType wrapType = StringToWrapType(wrapTypes[this->selectedWrap]);
			if (textureTypeString == "Diffuse")
			{
				Ref<DiffuseTexture> texture = TextureManager::LoadDiffuseTexture(ss.str(), filterType, wrapType, this->genMipMaps);
				this->meshData->AddTexture(texture);
			}
			else if (textureTypeString == "Heightmap")
			{
				Ref<HeightMapTexture> texture = TextureManager::LoadHeightmapTexture(ss.str(), filterType, wrapType, glm::vec3(0.0f, 0.0f, 0.0f), 1000.0f); 
				this->meshData->AddTexture(texture);
			}
			else if (textureTypeString == "Discard")
			{
				Ref<DiscardTexture> texture = TextureManager::LoadDiscardTexture(ss.str(), filterType, wrapType, this->genMipMaps);
				this->meshData->AddTexture(texture);
			}
			else if (textureTypeString == "Alpha")
			{
				Ref<AlphaTexture> texture = TextureManager::LoadAlphaTexture(ss.str(), filterType, wrapType, this->genMipMaps);
				this->meshData->AddTexture(texture);
			}
		}

		ImGui::EndChild();	
	}

	if (this->light)
	{
		ImGui::NewLine();
		ImGui::DragFloat4("Light Position", (float*)&this->light->light->position);

		ImGui::NewLine();
		ImGui::DragFloat4("Diffuse", (float*)&this->light->light->diffuse, 0.01f);

		ImGui::NewLine();
		ImGui::DragFloat4("Specular", (float*)&this->light->light->specular, 0.01f);

		ImGui::NewLine();
		ImGui::DragFloat4("Attenuation", (float*)&this->light->light->attenuation, 0.001f);

		ImGui::NewLine();
		ImGui::DragFloat4("Direction", (float*)&this->light->light->direction, 0.01f);

		ImGui::NewLine();
		ImGui::DragFloat("InnerAngle", (float*)&this->light->light->innerAngle, 0.01f);

		ImGui::NewLine();
		ImGui::DragFloat("OuterAngle", (float*)&this->light->light->outerAngle, 0.01f);

		ImGui::NewLine();
		ImGui::InputText("Light Type", &this->lightType);

		if (ImGui::Button("Add Flicker"))
		{
			this->light->attachements.push_back(CreateRef<FlickerAttachment>(this->light->light));
		}


		if (ImGui::Button("Change Light Type"))
		{
			if (this->lightType == "DIRECTIONAL")
			{
				this->light->light->lightType = Light::LightType::DIRECTIONAL;
			}
			else if (this->lightType == "POINT")
			{
				this->light->light->lightType = Light::LightType::POINT;
			}
			else if (this->lightType == "SPOT")
			{
				this->light->light->lightType = Light::LightType::SPOT;
			}
		}

		this->light->light->SendToShader();

		ImGui::NewLine();
		ImGui::Checkbox("State", &this->light->light->state);
	}

	if (ImGui::Button("Add Light"))
	{
		scene->AddLight(glm::vec3(0.0f, 0.0f, 0.0f));
	}

	ImGui::Text("Load Scene");
	ImGui::InputText("Load Scene Name", &this->sceneLoadName);
	if (ImGui::Button("Load"))
	{
		this->scene->Load(this->sceneLoadName);
	}

	ImGui::Text("");
	ImGui::Text("Save Scene");
	ImGui::InputText("Save Scene Name", &this->sceneName);
	if (ImGui::Button("Save"))
	{
		this->scene->Save(this->sceneName);
	}

	ImGui::End();
}