#version 330 core
in vec3 tex_coords;
out vec4 color;

uniform samplerCube cubemap;

void main()
{
	color = texture(cubemap, tex_coords);
}