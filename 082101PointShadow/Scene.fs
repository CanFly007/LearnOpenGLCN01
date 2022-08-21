#version 330 core
out vec4 FragColor;

uniform sampler2D woodTexture;
uniform samplerCube depthCubemap;

uniform vec3 worldLightPos;
uniform vec3 worldCameraPos;
uniform float far_plane;

in V2F
{
	vec3 worldPos;
	vec3 worldNormal;
	vec2 uv;
}fs_in;

float ShadowCalcution(vec3 worldPos)
{
	vec3 frag2Light = worldPos - worldLightPos;
	float closetDepth = texture(depthCubemap,frag2Light).r;
	closetDepth *= far_plane;
	float currentDepth = length(frag2Light);
	float bias = 0.05;
	float shadow = currentDepth - bias > closetDepth ? 1.0 : 0.0;
	return shadow;
	//return closetDepth / far_plane;
}

void main()
{
	vec3 color = texture(woodTexture,fs_in.uv).rgb;
	vec3 worldNormal = normalize(fs_in.worldNormal);
	vec3 worldLightDir = normalize(worldLightPos - fs_in.worldPos);
	vec3 worldViewDir = normalize(worldCameraPos - fs_in.worldPos);
	vec3 halfDir = normalize(worldLightDir + worldViewDir);

	vec3 ambient = color * 0.3;
	vec3 diffuse = color * max(dot(worldNormal,worldLightDir),0.0);
	vec3 specular = color * pow(max(dot(worldNormal,halfDir),0.0),64.0);

	float shadow = ShadowCalcution(fs_in.worldPos);
	vec3 c = ambient + (1 - shadow) * (diffuse + specular);
	FragColor = vec4(c,1);
}