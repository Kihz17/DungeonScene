#pragma once

#include "pch.h"

#include <yaml-cpp/yaml.h>
#include <glm/glm.hpp>
#include <sstream>

class SerializeUtils
{
public:
	static std::string SavePath(const std::string& path)
	{
		std::stringstream ss;
		ss << SOLUTION_DIR;
		return path.substr(ss.str().size());
	}

	static std::string LoadPath(const std::string& path)
	{
		std::stringstream ss;
		ss << SOLUTION_DIR << path;
		return ss.str();
	}
};