#version 330 core
layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormals;

out vec2 uvs;
out vec3 normals;
out vec3 fragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vec4 fragPosTemp = view * model * vec4(vertexPos, 1.0);
	gl_Position = projection * fragPosTemp;

	uvs = vertexUV;
	fragPos = fragPosTemp.xyz;
	normals = vec3(transpose(inverse(model)) * vec4(vertexNormals, 1.0));
}