#include "Mesh.h"

Mesh::Mesh(std::vector <Vertex>& vertices, std::vector <GLuint>& indices, std::vector <Texture>& textures)
{
	Mesh::vertices = vertices;
	Mesh::indices = indices;
	Mesh::textures = textures;

	VAO.Bind();
	// Generates Vertex Buffer Object and links it to vertices
	VBO VBO(vertices);
	// Generates Element Buffer Object and links it to indices
	EBO EBO(indices);
	// Links VBO attributes such as coordinates and colors to VAO
	VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	VAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
	VAO.LinkAttrib(VBO, 2, 2, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));

	// Unbind all to prevent accidentally modifying them
	VAO.Unbind();
	VBO.Unbind();
	EBO.Unbind();
}


void Mesh::DrawMesh(Shader& shader, Camera& camera)
{
	// Bind shader to be able to access uniforms
	//shader.Activate();
	VAO.Bind();

	// Keep track of how many of each type of textures we have
	unsigned int numDiffuse = 0;
	unsigned int numSpecular = 0;
	unsigned int numHeight = 0;
	unsigned int numNormal = 0;

	for (unsigned int i = 0; i < textures.size(); i++)
	{
		std::string num;
		std::string type = textures[i].type;
		if (type == "diffuse")
		{
			num = std::to_string(numDiffuse++);
		}
		else if (type == "specular")
		{
			num = std::to_string(numSpecular++);
		}
		else if (type == "normal")
		{
			num = std::to_string(numNormal++);
		}
		else if (type == "height") 
		{
			num = std::to_string(numHeight++); 
		}

		//textures[i].texUnit(shader, (type + num).c_str(), i);
	}
	
	GLuint dc = glGetUniformLocation(shader.ID, "diffuseCount");
	glUniform1i(dc, numDiffuse);

	for (unsigned int i = 0; i < numDiffuse; i++)
	{
		std::string name = "diffuse[" + std::to_string(i) + "]";
		textures[i].texUnit(shader, name.c_str(), i);
		textures[i].Bind();
	}

	GLuint sc = glGetUniformLocation(shader.ID, "specularCount");
	glUniform1i(sc, numSpecular);
	for (unsigned int i = 0; i < numSpecular; i++)
	{
		std::string name = "specular[" + std::to_string(i) + "]";
		textures[i].texUnit(shader, name.c_str(), i);
		textures[i].Bind();
	}
	
	// Take care of the camera Matrix
	//glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
	camera.createMatrix(shader, "camMatrix");

	// Draw the actual mesh
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	for (unsigned int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glActiveTexture(GL_TEXTURE0);

}