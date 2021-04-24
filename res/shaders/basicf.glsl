#version 330 core
in vec2 uvs;
in vec3 normals;
out vec4 color;

uniform sampler2D tex;

void main()
{
	color = vec4((normals + vec3(1.0, 1.0, 1.0)) * vec3(0.5, 0.5, 0.5), 1.0);
}