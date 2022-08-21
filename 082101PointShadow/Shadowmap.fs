#version 330 core
in vec4 worldPos;

uniform float far_plane;
uniform vec3 worldLightPos;

void main()
{
	float lightDistance = length(worldPos.xyz - worldLightPos);
	lightDistance = lightDistance / far_plane;
	gl_FragDepth = lightDistance;
}