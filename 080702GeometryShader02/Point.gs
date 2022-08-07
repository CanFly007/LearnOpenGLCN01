#version 330 core
layout (triangles) in;//来的是个三角形
layout(line_strip,max_vertices = 6) out;//输出的是六个顶点，三条线段（对应三角形三条法线）

in VS_OUT
{
	vec3 normal;
}gs_in[];

const float MAGNITUDE = 0.04;

void GenerateLine(int index)
{
	gl_Position = gl_in[index].gl_Position;//模型上的顶点
	EmitVertex();//生成出来
	gl_Position = gl_in[index].gl_Position + vec4(gs_in[index].normal ,0.0) * MAGNITUDE;//向法线方向延申出来的顶点
	EmitVertex();//生成出来
	EndPrimitive();//形成line_strip线段
}

void main()
{
	//gl_in在这里就是有三个顶点的数组，上面的例子就只有一个点，看输入的图元
	GenerateLine(0); // 第一个顶点法线
    GenerateLine(1); // 第二个顶点法线
    GenerateLine(2); // 第三个顶点法线
}