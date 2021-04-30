#version 330 core
in vec2 uvs;
in vec3 normals;
in vec3 fragPos;
out vec4 color;

uniform vec3 col;

void main()
{
	vec3 lightPos = vec3(17.5, 30.0, 17.5);
	vec3 lightDir = normalize(lightPos - fragPos);
	float diff = max(0, dot(normals, lightDir));

	vec3 ambient = col * 0.1;
	vec3 diffuse = col * diff;
	color = vec4(ambient + diffuse, 1.0);
	color.a = 0.3;
}