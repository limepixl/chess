#include "asset_management.hpp"
#include <stdio.h>
#include <glad/glad.h>
#include <stb_image.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <vector>

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

Texture LoadTextureFromFile(const char *path)
{
	stbi_set_flip_vertically_on_load(1);

	int width, height, channels;
	unsigned char *data = stbi_load(path, &width, &height, &channels, 0);
	if(!data)
	{
		printf("Failed to load texture from: %s\n", path);
		return {0};
	}

	int index = Texture::numTexturesLoaded++;

	unsigned int ID;
	glGenTextures(1, &ID);

	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, ID);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	if(channels == 3)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	else if(channels == 4)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	else
		printf("This number of channels is unsupported currently!\n");

	glGenerateMipmap(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(data);
	return {ID, index, width, height, channels};
}

Mesh LoadMeshFromOBJ(const char *path)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warning, error;
	bool loaded = tinyobj::LoadObj(&attrib, &shapes, &materials, &error, path);
	if(!loaded)
	{
		printf("Failed to load OBJ file from path: %s\n", path);
	}

	materials.push_back(tinyobj::material_t());
	std::vector<float> &vertices = attrib.vertices;
	std::vector<float> &uvs = attrib.texcoords;
	std::vector<float> &normals = attrib.normals;

	std::vector<float> finalVertices;
	std::vector<float> finalUVs;
	std::vector<float> finalNormals;
	finalVertices.reserve(vertices.size());
	finalUVs.reserve(uvs.size());
	finalNormals.reserve(normals.size());

	for(auto &shape : shapes)
	{
		for(auto &index : shape.mesh.indices)
		{
			finalVertices.push_back(vertices[3 * index.vertex_index]);
			finalVertices.push_back(vertices[3 * index.vertex_index + 1]);
			finalVertices.push_back(vertices[3 * index.vertex_index + 2]);

			if(uvs.size() != 0)
			{
				finalUVs.push_back(uvs[2 * index.texcoord_index]);
				finalUVs.push_back(uvs[2 * index.texcoord_index + 1]);
			}

			if(normals.size() != 0)
			{
				finalNormals.push_back(normals[3 * index.normal_index]);
				finalNormals.push_back(normals[3 * index.normal_index + 1]);
				finalNormals.push_back(normals[3 * index.normal_index + 2]);
			}
		}
	}

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO[3];
	glGenBuffers(3, VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, finalVertices.size() * sizeof(float), finalVertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

	if(uvs.size() != 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, finalUVs.size() * sizeof(float), finalUVs.data(), GL_STATIC_DRAW);
		
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
	}

	if(normals.size() != 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
		glBufferData(GL_ARRAY_BUFFER, finalNormals.size() * sizeof(float), finalNormals.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	printf("Loaded mesh: %s\nTriangle count: %d\n", shapes[0].name.c_str(), (int)finalVertices.size() / 3);
	return {VAO, {VBO[0], VBO[1], VBO[2]}, finalVertices.size()};
}