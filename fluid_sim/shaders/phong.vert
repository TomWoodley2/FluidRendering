#version 430

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

out vec3 vertPos; //Vertex position in eye coords
out vec3 N; //Transformed normal
out vec3 lightPos; //Light position in eye coords



uniform vec3 LightPosition; // Light position 
uniform mat3 NormalMatrix;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{

   vertPos = vec3(view * model * vec4(VertexPosition,1.0)); 
   lightPos = vec3(view * model * vec4(LightPosition,1.0));  

   N = normalize(NormalMatrix * VertexNormal);
   //N = VertexNormal;
      
   gl_Position  = projection * view * model * vec4(VertexPosition,1.0);
}
