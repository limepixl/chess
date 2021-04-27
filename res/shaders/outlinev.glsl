#version 330 core
layout(location = 0) in vec3 vertex_pos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
	gl_Position = projection * view * model * vec4(vertex_pos, 1.0);
}