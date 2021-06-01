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
	glm::vec3 tint;
};

struct Scene
{
	std::vector<Mesh> meshes;
	std::vector<Entity> entities;
	std::vector<Entity> ghosts;
};

void DrawScene(Scene *scene, struct Shader *shader, struct Shader *ghostShader, struct State *state);
void DrawAABBs(std::vector<Entity> &entities, std::vector<Mesh> &meshes, struct Shader *shader);

struct Camera
{
	glm::vec3 cameraPos;
	glm::vec3 destination;
	glm::vec3 dir;
	glm::vec3 right;
	glm::vec3 up;
	glm::mat4 view;
	float t = 0.0f;
	float xRotate = 0.0f;
	float zRotate = 0.0f;
};

void UpdateBoard(glm::vec3 &rayWorld, glm::vec3 &cameraPos, Scene &scene, struct State &state);
void RotateBoard(State &state, Camera &cam);
bool RayHit(glm::vec3 &rayOrigin, glm::vec3 &rayDir, std::vector<Entity> &entities, Entity **hit);
void GenerateGhostsOnGrid(struct State *state, Scene *scene);