#pragma once
#include "shader.hpp"
#include "texture.hpp"

Shader LoadShadersFromFiles(const char *vertexPath, const char *fragmentPath);
Texture LoadTextureFromFile(const char *path);