#version 430

in vec3 vertPos;
in vec3 N; //
in vec3 lightPos;

uniform vec3 CameraPosition; // Position of the camera

uniform vec3 Ia;			// Intensity of the ambient light
uniform vec3 Ip;            // Intensity of the point light source

// Material Reflection Coefficients
uniform vec3 Ka;			// Ambient 
uniform vec4 colour;        // Diffuse 
uniform vec3 Ks;			// Specular



layout( location = 0 ) out vec4 FragColour;

void main() 
{
   vec3 Kd = vec3(colour.r,colour.g,colour.b);
   vec3 lightIntensity; // Final light intensity to be returned	
   
   // Calculate the vector between the light source and the surface point
   vec3 L = normalize(lightPos - vertPos);  
   
   // Calculate the direction of reflection 
   vec3 R = reflect(-L, N);
   
   // Calculate the direction to the viewpoint
   vec3 V = normalize(CameraPosition - vertPos);
   
   // Calculate distance between light source and point
   float D = length(lightPos - vertPos);
   
   // Calculate distance between camera and point
   float Dv = length(CameraPosition - vertPos);
   
   // Ambient term
   vec3 ambient;
   ambient = Ia * Ka;
   
   // Diffuse term
   vec3 diffuse;
   diffuse = Ip * Kd * dot(N,L);
   diffuse = clamp(diffuse, 0.0f, 1.0f);
   
   // Specular term
   vec3 specular;
   float n = 2; // Specular Reflection Exponent
   specular = Ip * Ks * pow((dot(R,V)),n);
   
   // Attenuation
   float attenuation;
   float c1, c2, c3; // Constants to adjust how attenuation affects the object
   c1 = 1;
   c2 = 0;
   c3 = 0;
   attenuation = 1/(c1 + (c2 * (D+Dv)) + (c3 * (D+Dv)* (D*Dv)));
   
   lightIntensity = ambient + diffuse + specular;
   //lightIntensity = diffuse;
   lightIntensity = clamp(lightIntensity, 0.0f, 1.0f);
   FragColour = vec4(lightIntensity, colour.a);
	
}
