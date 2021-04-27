#include "math.hpp"
#include "../display/display.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <SDL.h>

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

void CastRay(Display *display, glm::mat4 &projection, glm::mat4 &view)
{
	int xtmp, ytmp;
	SDL_GetMouseState(&xtmp, &ytmp);

	// https://antongerdelan.net/opengl/raycasting.html
	// Convert x and y to normalized device coordinates
	float x = (2.0f * xtmp) / display->width - 1.0f;
	float y = 1.0f - (2.0f * ytmp) / display->height;

	// 4D Homogeneous clip coordinates
	glm::vec4 rayClip(x, y, -1.0f, 1.0f);

	// 4D Camera coordinates
	glm::vec4 rayCam = glm::inverse(projection) * rayClip;
	// unproject xy
	rayCam.z = -1.0f;
	rayCam.w = 0.0f;

	// World coordinates
	glm::vec3 rayWorld = glm::vec3(glm::inverse(view) * rayCam);
	rayWorld = glm::normalize(rayWorld);
}