#version 330 core
out vec4 FragColor;

in vec2 uv;
in vec3 worldPos;
in vec3 worldNormal;

//uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 worldlightPos;
uniform vec3 worldViewPos;

struct Material
{
	sampler2D diffuseTex;
	sampler2D specularMaskTex;
	float gloss;
};
uniform Material material;


//uniform sampler2D diffuseTex;

void main()
{
	vec3 diffuseColor = vec3(texture(material.diffuseTex, uv).xyz);

	vec3 ambient = 0.1 * lightColor * diffuseColor;

	vec3 normal = normalize(worldNormal);
	vec3 lightDir = normalize(worldlightPos - worldPos);
	vec3 diffuse = lightColor * max((dot(normal , lightDir)),0) * diffuseColor;

	vec3 worldViewDir = normalize(worldViewPos - worldPos);
	vec3 reflectDir = reflect(-lightDir , normal);
	vec3 specMask = vec3(texture(material.specularMaskTex, uv).xyz);
	vec3 specular = 0.5 * specMask * pow(max(dot(worldViewDir , reflectDir), 0), material.gloss);

	vec3 result = ambient + diffuse + specular;

	FragColor = vec4(result , 1.0);
}