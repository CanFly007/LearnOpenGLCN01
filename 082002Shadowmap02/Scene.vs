#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out VS_OUT
{
	vec3 worldPos;
	vec3 worldNormal;
	vec2 uv;
	vec4 lightSpacePos;
}vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main()
{
	vs_out.worldPos = vec3(model * vec4(position,1.0));
	vs_out.worldNormal = transpose(inverse(mat3(model))) * normal;
	vs_out.uv = texCoords;
	vs_out.lightSpacePos = lightSpaceMatrix * vec4(vs_out.worldPos,1.0);
	
	gl_Position = projection * view * model * vec4(position,1.0f);
}