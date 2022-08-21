#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

uniform mat4 lightSpacePVs[6];

out vec4 worldPos;

void main()
{
	for(int face = 0; face < 6; face++)
	{
		gl_Layer = face;// built-in variable that specifies to which face we render.
		for(int i = 0; i < 3; i++)
		{
			worldPos = gl_in[i].gl_Position;
			gl_Position = lightSpacePVs[face] * worldPos;
			EmitVertex();
		}
		EndPrimitive();
	}
}