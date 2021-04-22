#pragma once
#include <unordered_map>
#include <string>

struct Shader
{
	unsigned int ID;
	std::unordered_map<std::string, int> uniforms;
};

