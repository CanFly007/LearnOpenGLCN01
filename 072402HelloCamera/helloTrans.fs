#version 330 core
out vec4 FragColor;

//in vec3 ourColor;
in vec2 uv;

uniform sampler2D containerTex;
uniform sampler2D smileTex;

void main()
{
	FragColor = mix(texture(containerTex, uv), texture(smileTex, uv), 0.2);
}
