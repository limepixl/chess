#include "asset_management.hpp"
#include <stdio.h>
#include <glad/glad.h>
#include <stb_image.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <vector>
#include "../math/math.hpp"

void PrepareAABBForRendering(AABB aabb, unsigned int *VAO)
{
	float x1 = aabb.p1.x;
	float x2 = aabb.p2.x;
	float y1 = aabb.p1.y;
	float y2 = aabb.p2.y;
	float z1 = aabb.p1.z;
	float z2 = aabb.p2.z;

	float vertices[]
	{
		// front
		x1, y1, z2,
		x2, y1, z2,
		x2, y2, z2,
		x2, y2, z2,
		x1, y2, z2,
		x1, y1, z2,

		// left
		x2, y1, z2,
		x2, y1, z1,
		x2, y2, z1,
		x2, y2, z1,
		x2, y2, z2,
		x2, y1, z2,

		// right
		x1, y1, z1,
		x1, y1, z2,
		x1, y2, z2,
		x1, y2, z2,
		x1, y2, z1,
		x1, y1, z1,

		// back
		x2, y1, z1,
		x1, y1, z1,
		x1, y2, z1,
		x1, y2, z1,
		x2, y2, z1,
		x2, y1, z1,

		// top
		x2, y2, z1,
		x1, y2, z1,
		x1, y2, z2,
		x1, y2, z2,
		x2, y2, z2,
		x2, y2, z1,

		// bottom
		x1, y1, z1,
		x2, y1, z1,
		x2, y1, z2,
		x2, y1, z2,
		x1, y1, z2,
		x1, y1, z1
	};

	glGenVertexArrays(1, VAO);
	glBindVertexArray(*VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
}

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
	if(!fread(buffer, size * sizeof(char), 1, vertexRaw))
		printf("ERROR: FILE\n");
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
	if(!fread(buffer, size * sizeof(char), 1, fragmentRaw))
		printf("ERROR: FILE\n");
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
		int len, s;
		GLenum type;
		glGetActiveUniform(ID, i, 512, &len, &s, &type, name);
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
		return {};
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
	printf("Loaded texture: %s\n", path);
	return {ID, index, width, height, channels};
}

Mesh LoadMesh(const char *path)
{
	std::string binaryFile(path);
	binaryFile += ".bin";
	FILE *binary = fopen(binaryFile.c_str(), "rb");
	if(!binary)
	{
		return LoadMeshFromOBJ(path);
	}

	std::vector<float> vertices, uvs, normals;

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO[3];
	glGenBuffers(3, VBO);

	int numVertices, numUVs, numNormals;
	if(fscanf(binary, "%d %d %d\n", &numVertices, &numUVs, &numNormals) == EOF)
		printf("ERROR: FILE\n");

	float *buffer = new float[numVertices];
	if(!fread(buffer, sizeof(float), numVertices, binary))
		printf("ERROR: FILE\n");
	vertices = std::vector<float>(buffer, buffer + numVertices);
	delete[] buffer;

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

	if(numUVs != 0)
	{
		buffer = new float[numUVs];
		if(!fread(buffer, sizeof(float), numUVs, binary))
			printf("ERROR: FILE\n");
		uvs = std::vector<float>(buffer, buffer + numUVs);
		delete[] buffer;

		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(float), uvs.data(), GL_STATIC_DRAW);
		
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
	}

	if(numNormals != 0)
	{
		buffer = new float[numNormals];
		if(!fread(buffer, sizeof(float), numNormals, binary))
			printf("ERROR: FILE\n");
		normals = std::vector<float>(buffer, buffer + numNormals);
		delete[] buffer;

		glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	}

	fclose(binary);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	unsigned int AABB_VAO;
	AABB tmp = ComputeAABBFromVertices(vertices);
	PrepareAABBForRendering(tmp, &AABB_VAO);

	printf("Loaded mesh from cache: %s\nVertex count: %d\n", path, (int)vertices.size());
	return {VAO, {VBO[0], VBO[1], VBO[2]}, vertices.size(), tmp, AABB_VAO};
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

	// Make cache
	std::string binaryPath(path);
	binaryPath += ".bin";
	FILE *binary = fopen(binaryPath.c_str(), "wb");

	fprintf(binary, "%lu %lu %lu\n", (unsigned long)finalVertices.size(), (unsigned long)finalUVs.size(), (unsigned long)finalNormals.size());
	fwrite(finalVertices.data(), sizeof(float), finalVertices.size(), binary);

	if(uvs.size() != 0)
		fwrite(finalUVs.data(), sizeof(float), finalUVs.size(), binary);

	if(normals.size() != 0)
		fwrite(finalNormals.data(), sizeof(float), finalNormals.size(), binary);

	fclose(binary);

	unsigned int AABB_VAO;
	AABB tmp = ComputeAABBFromVertices(finalVertices);
	PrepareAABBForRendering(tmp, &AABB_VAO);

	printf("Loaded mesh: %s\nVertex count: %d\n", shapes[0].name.c_str(), (int)finalVertices.size());
	return {VAO, {VBO[0], VBO[1], VBO[2]}, finalVertices.size(), tmp, AABB_VAO};
}

Scene LoadSceneFromFile(const char *path)
{
	FILE *sceneRaw = fopen(path, "rb");
	if(!sceneRaw)
	{
		printf("Failed to open scene file: %s!\n", path);
	}

	std::vector<Mesh> meshes;
	std::vector<Entity> entities;

	char lineType;
	while((lineType = (char)fgetc(sceneRaw)) != EOF)
	{
		switch(lineType)
		{
			case 'm':
			{
				char meshName[100];
				if(fscanf(sceneRaw, " %s\n", meshName) == EOF)
					printf("ERROR: FILE\n");

				char tmp[100] = "res/models/";
				meshes.push_back(LoadMesh(strcat(tmp, meshName)));
				break;
			}

			case 'e':
			{
				int meshIndex;
				float tx, ty, tz;
				float rx, ry, rz;
				float sx, sy, sz;
				int side;
				if(fscanf(sceneRaw, " %d %f %f %f %f %f %f %f %f %f %d\n", &meshIndex, &tx, &ty, &tz, &rx, &ry, &rz, &sx, &sy, &sz, &side)
					== EOF)
					printf("ERROR: FILE\n");
				glm::vec3 translation(tx, ty, tz);
				glm::vec3 rotation(rx, ry, rz);
				glm::vec3 scale(sx, sy, sz);
				entities.push_back({meshIndex, translation, rotation, scale, side});
				break;
			}

			default:
			{
				printf("Character %c currently unsupported!\n", lineType);
				break;
			}
		}
	}

	printf("Finished loading scene file: %s\n", path);
	fclose(sceneRaw);
	return {meshes, entities};
}