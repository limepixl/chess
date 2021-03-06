#version 330 core
in vec2 uvs;
in vec3 normals;
in vec3 fragPos;
out vec4 color;

uniform sampler2D tex;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
	vec3 col = texture(tex, uvs).xyz;
	vec3 lightDir = normalize(lightPos - fragPos);
	float diff = max(0, dot(normals, lightDir));

	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 reflectedDir = reflect(-lightDir, normals);
	float spec = pow(max(0, dot(reflectedDir, viewDir)), 32);

	vec3 ambient = col * 0.1;
	vec3 diffuse = col * diff;
	vec3 specular = col * spec;
	color = vec4(ambient + diffuse, 1.0);
}