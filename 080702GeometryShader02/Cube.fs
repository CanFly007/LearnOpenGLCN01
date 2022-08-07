#version 330 core
out vec4 FragColor;

uniform sampler2D _Diffuse1;

in vec2 uv;

void main()
{
	vec3 diffuseTex = vec3(texture(_Diffuse1,uv));
    FragColor = vec4(diffuseTex.xyz, 1.0);   
}