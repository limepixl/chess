#version 330 core
layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec2 vertexUV;

out vec2 uvs;

void main()
{
	uvs = vertexUV;
	gl_Position = vec4(vertexPos, 1.0);
}