#pragma once
#include "shader.hpp"
#include "texture.hpp"
#include "scene.hpp"

Shader LoadShadersFromFiles(const char *vertexPath, const char *fragmentPath);
Texture LoadTextureFromFile(const char *path);

Mesh LoadMeshFromOBJ(const char *path);
Mesh LoadMesh(const char *path);
Scene LoadSceneFromFile(const char *path);