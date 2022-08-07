#version 330 core
layout (triangles) in;//�����Ǹ�������
layout(line_strip,max_vertices = 6) out;//��������������㣬�����߶Σ���Ӧ�������������ߣ�

in VS_OUT
{
	vec3 normal;
}gs_in[];

const float MAGNITUDE = 0.04;

void GenerateLine(int index)
{
	gl_Position = gl_in[index].gl_Position;//ģ���ϵĶ���
	EmitVertex();//���ɳ���
	gl_Position = gl_in[index].gl_Position + vec4(gs_in[index].normal ,0.0) * MAGNITUDE;//���߷�����������Ķ���
	EmitVertex();//���ɳ���
	EndPrimitive();//�γ�line_strip�߶�
}

void main()
{
	//gl_in�����������������������飬��������Ӿ�ֻ��һ���㣬�������ͼԪ
	GenerateLine(0); // ��һ�����㷨��
    GenerateLine(1); // �ڶ������㷨��
    GenerateLine(2); // ���������㷨��
}