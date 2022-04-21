#pragma once

#include "Mesh.h"

#include <unordered_map>

class MeshManager
{
public:
	static Ref<Mesh> LoadMesh(const std::string& path, bool copy = false);

private:
	static std::unordered_map<std::string, Ref<Mesh>> loadedMeshes;
};