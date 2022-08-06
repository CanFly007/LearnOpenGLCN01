#version 330 core
layout(location = 0) in vec3 aPos;

out vec3 uvw;

uniform mat4 projection;
uniform mat4 view;

void main()
{
	uvw = aPos; //世界坐标的位置，就是uv值
	gl_Position = projection * view * vec4(aPos,1.0); //没有model矩阵
	gl_Position = vec4(gl_Position.xy,gl_Position.w,gl_Position.w); //齐次除法后，NDC_z的值就变为了1
}