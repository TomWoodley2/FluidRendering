#version 330 core 

out vec4 FragColor; 

uniform vec3 colour;
								  
void main()						  
{		
	//vec3 colour = vec3(0.5f,1.0f,0.5f);
	FragColor = vec4(colour, 1.0f); 
}									  