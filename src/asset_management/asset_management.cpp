#include "asset_management.hpp"
#include <stdio.h>
#include <glad/glad.h>
#include <stb_image.h>

Shader LoadShadersFromFiles(const char *vertexPath, const char *fragmentPath)
{
	FILE *vertexRaw = fopen(vertexPath, "rb");
	if(!vertexRaw)
		printf("Failed to open vertex shader from path: %s\n", vertexPath);

	// Get file size in bytes
	fseek(vertexRaw, 0, SEEK_END);
	long size = ftell(vertexRaw);
	rewind(vertexRaw);

	char *buffer = new char[size+1];
	fread(buffer, size * sizeof(char), 1, vertexRaw);
	buffer[size] = '\0';
	fclose(vertexRaw);

	unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &buffer, nullptr);
	glCompileShader(vertex);
	delete[] buffer;

	GLint compiled;
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &compiled);
	if(compiled != GL_TRUE)
	{
	    GLsizei log_length = 0;
	    GLchar message[1024];
	    glGetShaderInfoLog(vertex, 1024, &log_length, message);
		printf("VERTEX SHADER COMPILE ERROR:\n%s\n", message);
	}

	FILE *fragmentRaw = fopen(fragmentPath, "rb");
	if(!fragmentRaw)
		printf("Failed to open fragment shader from path: %s\n", fragmentPath);

	// Get file size in bytes
	fseek(fragmentRaw, 0, SEEK_END);
	size = ftell(fragmentRaw);
	rewind(fragmentRaw);

	buffer = new char[size+1];
	fread(buffer, size * sizeof(char), 1, fragmentRaw);
	buffer[size] = '\0';
	fclose(fragmentRaw);

	unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &buffer, nullptr);
	glCompileShader(fragment);
	delete[] buffer;

	glGetShaderiv(fragment, GL_COMPILE_STATUS, &compiled);
	if(compiled != GL_TRUE)
	{
	    GLsizei log_length = 0;
	    GLchar message[1024];
	    glGetShaderInfoLog(fragment, 1024, &log_length, message);
		printf("FRAGMENT SHADER COMPILE ERROR:\n%s\n", message);
	}

	unsigned int ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);

	// Clean up
	glDetachShader(ID, vertex);
	glDetachShader(ID, fragment);
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	// Find all uniforms
	int numActiveUniforms;
	glGetProgramiv(ID, GL_ACTIVE_UNIFORMS, &numActiveUniforms);

	std::unordered_map<std::string, int> uniforms;

	char name[512];
	for(int i = 0; i < numActiveUniforms; i++)
	{
		int len, size;
		GLenum type;
		glGetActiveUniform(ID, i, 512, &len, &size, &type, name);
		uniforms[std::string(name)] = glGetUniformLocation(ID, name);
	}

	return {ID, uniforms};
}