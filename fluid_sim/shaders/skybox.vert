#version 330

layout (location=0) in vec3 VertexPosition;

out vec3 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	TexCoords = VertexPosition;
	//texCoords = VertexPosition;
    gl_Position =  projection * view * model *  vec4(VertexPosition,1.0);
}