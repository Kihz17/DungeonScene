#include "MeshManager.h"

std::unordered_map<std::string, Ref<Mesh>> MeshManager::loadedMeshes;

Ref<Mesh> MeshManager::LoadMesh(const std::string& path, bool copy)
{
	std::unordered_map<std::string, Ref<Mesh>>::iterator it = loadedMeshes.find(path);
	if (it != loadedMeshes.end())
	{
		if (copy)
		{
			return CreateRef<Mesh>(path); // We don't really need the manager to store this since it creates a shared ptr that will be destroyed on its own when out of scope
		}

		return it->second;
	}

	Ref<Mesh> mesh = CreateRef<Mesh>(path);
	loadedMeshes.insert(std::make_pair(path, mesh));
	return mesh;
}
