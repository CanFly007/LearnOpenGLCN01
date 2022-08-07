#version 330 core
layout (points) in;//来的是顶点
layout (triangle_strip,max_vertices = 5) out;//输出的是五个顶点用triangle_strip方式连成的三角形

in VS_OUT
{
	vec3 color;
}gs_in[];

out vec3 fColor;

void main()
{
	fColor = gs_in[0].color;//设置为顶点色的状态

	gl_Position = gl_in[0].gl_Position + vec4(-0.2, -0.2, 0.0, 0.0);    // 1:左下
	EmitVertex();//生成一个顶点
	gl_Position = gl_in[0].gl_Position + vec4( 0.2, -0.2, 0.0, 0.0);    // 2:右下
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + vec4(-0.2,  0.2, 0.0, 0.0);    // 3:左上
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + vec4( 0.2,  0.2, 0.0, 0.0);    // 4:右上
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + vec4( 0.0,  0.4, 0.0, 0.0);    // 5:顶部
	fColor = vec3(1.0,1.0,1.0);//改变顶部的颜色再生成顶点
    EmitVertex();
    EndPrimitive();
}