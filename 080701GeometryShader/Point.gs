#version 330 core
layout (points) in;//�����Ƕ���
layout (triangle_strip,max_vertices = 5) out;//����������������triangle_strip��ʽ���ɵ�������

in VS_OUT
{
	vec3 color;
}gs_in[];

out vec3 fColor;

void main()
{
	fColor = gs_in[0].color;//����Ϊ����ɫ��״̬

	gl_Position = gl_in[0].gl_Position + vec4(-0.2, -0.2, 0.0, 0.0);    // 1:����
	EmitVertex();//����һ������
	gl_Position = gl_in[0].gl_Position + vec4( 0.2, -0.2, 0.0, 0.0);    // 2:����
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + vec4(-0.2,  0.2, 0.0, 0.0);    // 3:����
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + vec4( 0.2,  0.2, 0.0, 0.0);    // 4:����
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + vec4( 0.0,  0.4, 0.0, 0.0);    // 5:����
	fColor = vec3(1.0,1.0,1.0);//�ı䶥������ɫ�����ɶ���
    EmitVertex();
    EndPrimitive();
}