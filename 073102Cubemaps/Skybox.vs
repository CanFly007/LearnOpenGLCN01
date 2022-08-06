#version 330 core
layout(location = 0) in vec3 aPos;

out vec3 uvw;

uniform mat4 projection;
uniform mat4 view;

void main()
{
	uvw = aPos; //���������λ�ã�����uvֵ
	gl_Position = projection * view * vec4(aPos,1.0); //û��model����
	gl_Position = vec4(gl_Position.xy,gl_Position.w,gl_Position.w); //��γ�����NDC_z��ֵ�ͱ�Ϊ��1
}