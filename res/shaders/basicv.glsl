#version 330 core
layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 normals;

out vec2 uvs;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	uvs = vertexUV;
	gl_Position = projection * view * model * vec4(vertexPos, 1.0);
}