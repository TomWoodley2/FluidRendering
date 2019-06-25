#version 330
//skybox.frag

in vec3 TexCoords; // Loads in the texture coordinates from vertex shader
out vec4 color; // Outputs a colour
uniform samplerCube cubemap; // Loads the cube map from the main code

void main()
{
	// Sets the colour of the pixel being shaded to the coordinate colour on cube map
	color = texture(cubemap, TexCoords);
	//color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}