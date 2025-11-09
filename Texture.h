#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include<glad/glad.h>
#include<stb/stb_image.h>

#include"shaderClass.h"
#include<string>
class Texture
{
public:
	GLuint ID;
	std::string type;
	std::string path;
	GLuint unit;

	Texture(GLuint id, std::string texType, std::string tex_path);

	// Assigns a texture unit to a texture
	void texUnit(Shader& shader, const char* uniform, GLuint unit);
	// Binds a texture
	void Bind();
	// Unbinds a texture
	void Unbind();
	// Deletes a texture
	void Delete();
};
#endif