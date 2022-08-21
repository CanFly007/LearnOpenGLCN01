#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoords;

out V2F
{
	vec3 worldPos;
	vec3 worldNormal;
	vec2 uv;
}vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
	gl_Position = projection * view * model * vec4(position,1.0f);
	vs_out.worldPos = vec3(model * vec4(position,1.0));
	vs_out.worldNormal = transpose(inverse(mat3(model))) * normal;
	vs_out.uv = texcoords;
}