#pragma once

#include "Mesh.h"

#include <glm/glm.hpp>

class Raycast
{
public:
	// https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
	static bool Cast(const glm::vec3& origin, const glm::vec3& ray, Mesh* geometry, const glm::vec3& translation, glm::vec3& intersectionPoint)
	{
		const float EPSILON = 1e-8;
		bool foundIntersect = false;

		for (const Face& face : geometry->GetFaces()) 
		{
			Vertex v1 = geometry->GetVertices()[face.v1];
			Vertex v2 = geometry->GetVertices()[face.v2];
			Vertex v3 = geometry->GetVertices()[face.v3];

			glm::vec3 vertexPos1 = v1.position + translation;
			glm::vec3 vertexPos2 = v2.position + translation;
			glm::vec3 vertexPos3 = v3.position + translation;

			glm::vec3 edge1 = vertexPos2 - vertexPos1;
			glm::vec3 edge2 = vertexPos3 - vertexPos1;
			glm::vec3 pvec = glm::cross(ray, edge2);
			float det = glm::dot(edge1, pvec);

			if(fabs(det) < EPSILON)
			if (det > -EPSILON && det < EPSILON) // We are parallel to the face
			{
				continue;
			}

			float invDet = 1.0f / det;

			glm::vec3 tVec = origin - vertexPos1;
			float u = invDet * glm::dot(tVec, pvec);
			if (u < 0.0f || u > 1.0f)
			{
				continue;
			}
		
			glm::vec3 qVec = glm::cross(tVec, edge1);
			float v = invDet * glm::dot(ray, qVec);
			if (v < 0.0f || (u + v) > 1.0f)
			{
				continue;
			}

			float t = invDet * glm::dot(edge2, qVec);
			glm::vec3 point = origin + ray * t;
			if (!foundIntersect)
			{
				foundIntersect = true;
				intersectionPoint = point;
			}
			else if (glm::distance(origin, point) < glm::distance(origin, intersectionPoint))
			{
				intersectionPoint = point;
			}
		}

		return foundIntersect;
	}

};