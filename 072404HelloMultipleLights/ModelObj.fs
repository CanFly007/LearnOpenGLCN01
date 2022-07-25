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

struct PointLight
{
	vec3 position;
	vec3 color;
};
uniform PointLight pointLights[4];

void main()
{
	vec3 diffuseColor = vec3(texture(material.diffuseTex, uv).xyz);
	vec3 specMask = vec3(texture(material.specularMaskTex, uv).xyz);	

	vec3 normal = normalize(worldNormal);
	vec3 worldViewDir = normalize(worldViewPos - worldPos);

	vec3 result = vec3(0,0,0);
	for(int i = 0; i < 4; i++)
	{
		float distance = length(pointLights[i].position - worldPos);
		float atten = 1 / (distance * distance * 0.3f);
		vec3 lightDir = normalize(pointLights[i].position - worldPos);
		vec3 pointDiffuse = pointLights[i].color * diffuseColor * max((dot(normal , lightDir)),0);
		vec3 halfDir = normalize(lightDir + worldViewDir);
		vec3 pointSpecular = pointLights[i].color * specMask * pow(max(dot(normal,halfDir),0),material.gloss);
		result += (pointDiffuse + pointSpecular) * atten;
	}

	FragColor = vec4(result , 1.0);
}