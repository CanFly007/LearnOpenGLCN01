#version 330 core
out vec4 FragColor;

in vec2 uv;

uniform sampler2D _Diffuse;

void main()
{    
    vec4 texColor = texture(_Diffuse, uv);
	FragColor = texColor;
}