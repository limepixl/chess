#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <vector>

glm::mat4 GetProjectionMatrix(struct Display *display);
float Lerp(float v0, float v1, float t);

struct AABB
{
	glm::vec3 p1;
	glm::vec3 p2;
};

AABB ComputeAABBFromVertices(std::vector<float>& vertices);

void CastRay(struct Display *display, glm::mat4 &projection, glm::mat4 &view);