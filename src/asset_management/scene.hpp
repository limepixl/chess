#pragma once
#include <vector>
#include <glm/vec3.hpp>

struct Mesh
{
	unsigned int VAO;
	unsigned int VBOs[3];
	size_t numVertices;
};


struct Entity
{
	int meshIndex;
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};

struct Scene
{
	std::vector<struct Mesh> meshes;
	std::vector<Entity> entities;
};

void DrawScene(Scene *scene, struct Shader *shader);