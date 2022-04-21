#pragma once

#include <glm/glm.hpp>

class AABB
{
public:
	AABB(const glm::vec3& min, const glm::vec3 max);
	virtual ~AABB();

	void Draw(const glm::vec3& position, const glm::vec3& scale) const;

	glm::vec3 min;
	glm::vec3 max;

private:
	float* GetVertices(const glm::vec3& position, const glm::vec3& scale) const;
};