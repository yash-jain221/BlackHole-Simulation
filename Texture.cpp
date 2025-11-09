#include"Texture.h"

Texture::Texture(GLuint id, std::string texType, std::string tex_path){
	// Assigns the type of the texture ot the texture object
	ID = id;
	type = texType;
	path = tex_path;
	unit = 0;
}

void Texture::texUnit(Shader& shader, const char* uniform, GLuint unit)
{	
	glActiveTexture(GL_TEXTURE0 + unit);
	// Gets the location of the uniform
	GLuint texUni = glGetUniformLocation(shader.ID, uniform);
	// Shader needs to be activated before changing the value of a uniform
	shader.Activate();
	// Sets the value of the uniform
	glUniform1i(texUni, unit);
}

void Texture::Bind()
{
	glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::Unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Delete()
{
	glDeleteTextures(1, &ID);
}