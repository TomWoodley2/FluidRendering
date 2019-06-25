#version 330 core 

uniform vec4 colour;

out vec4 FragColor; 
								  
void main()						  
{									  
	FragColor = vec4(colour.r, colour.g, colour.b, 1.0f); 
}									  