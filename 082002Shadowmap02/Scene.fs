#version 330 core
out vec4 FragColor;

in VS_OUT
{
	vec3 worldPos;
	vec3 worldNormal;
	vec2 uv;
	vec4 lightSpacePos;
}fs_in;

uniform sampler2D _Diffuse;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

float ShadowCalculation(vec4 lightSpacePos , vec3 worldNormal , vec3 worldLightDir)
{
	vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
	projCoords = projCoords * 0.5 + 0.5;
	float closestDepth = texture(shadowMap,projCoords.xy).r;
	float currentDepth = projCoords.z;

	float bias =  0;//max(0.05 * (1.0 - dot(worldNormal, worldLightDir)), 0.005);
	float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

	shadow = currentDepth - closestDepth;
	return shadow;
}

void main()
{
	vec3 color = texture(_Diffuse,fs_in.uv).rgb;
	vec3 worldNormal = normalize(fs_in.worldNormal);
	vec3 worldLightDir = normalize(lightPos - fs_in.worldPos);
	vec3 worldViewDir = normalize(viewPos - fs_in.worldPos);
	vec3 halfDir = normalize(worldLightDir + worldViewDir);

	vec3 ambient = 0.3 * color;
	vec3 diffuse = max(dot(worldLightDir,worldNormal),0.0) * vec3(1.0);
	vec3 specular = pow(max(dot(halfDir,worldNormal),0.0),64.0) * vec3(1.0);

	float shadow = ShadowCalculation(fs_in.lightSpacePos , worldNormal , worldLightDir);
	vec3 lighting = (ambient + /*(1.0 - shadow) * */(diffuse + specular)) * color;

	FragColor = vec4(lighting,1.0f);
	//FragColor = vec4(shadow,0,0,1);

}