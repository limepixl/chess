#pragma once
#include <vector>
#include <glm/vec3.hpp>
#include "../math/math.hpp"

struct Mesh
{
	unsigned int VAO;
	unsigned int VBOs[3];
	size_t numVertices;
	struct AABB boundingBox;
	unsigned int AABB_VAO;
};

struct Entity
{
	int meshIndex;
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
	int side;
};

struct Scene
{
	std::vector<Mesh> meshes;
	std::vector<Entity> entities;
};

void DrawScene(Scene *scene, struct Shader *shader, struct State *state);
void DrawAABBs(std::vector<Entity> &entities, std::vector<Mesh> &meshes, struct Shader *shader);

bool RayHit(glm::vec3 &rayOrigin, glm::vec3 &rayDir, std::vector<Entity> &entities, Entity **hit);