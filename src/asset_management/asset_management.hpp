#pragma once
#include "shader.hpp"
#include "texture.hpp"
#include <vector>

Shader LoadShadersFromFiles(const char *vertexPath, const char *fragmentPath);
Texture LoadTextureFromFile(const char *path);

struct Mesh
{
	unsigned int VAO;
	unsigned int VBOs[3];
	size_t numVertices;
};

Mesh LoadMeshFromOBJ(const char *path);