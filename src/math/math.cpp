#include "math.hpp"
#include "../display/display.hpp"
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 GetProjectionMatrix(struct Display *display)
{
	glm::mat4 projection = glm::perspective(glm::radians(40.0f), (float)display->width / display->height, 0.1f, 100.0f);
	return projection;
}

float Lerp(float v0, float v1, float t)
{
	return (1 - t) * v0 + t * v1;
}

AABB ComputeAABBFromVertices(std::vector<float>& vertices)
{
	glm::vec3 min(INFINITY, INFINITY, INFINITY);
	glm::vec3 max(-INFINITY, -INFINITY, -INFINITY);
	for(size_t i = 0; i < vertices.size(); i+=3)
	{
		float v1 = vertices[i];
		float v2 = vertices[i + 1];
		float v3 = vertices[i + 2];

		if(min[0] > v1) min[0] = v1;
		if(min[1] > v2) min[1] = v2;
		if(min[2] > v3) min[2] = v3;
		if(max[0] < v1) max[0] = v1;
		if(max[1] < v2) max[1] = v2;
		if(max[2] < v3) max[2] = v3;
	}

	return {min, max};
}