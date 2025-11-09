#ifndef MESH_CLASS_H
#define MESH_CLASS_H

#include<string>
#include<vector>
#include"VAO.h"
#include"EBO.h"
#include"Camera.h"
#include"Texture.h"
#include<glm/glm.hpp>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>       // Output data structure
#include <assimp/postprocess.h>

class Mesh
{
public:
	std::vector<Vertex> vertices;
	std::vector <unsigned int> indices ;
	std::vector <Texture> textures;
	// Store VAO in public so it can be used in the Draw function
	VAO VAO;

	// Initializes the mesh
	Mesh(std::vector<Vertex> &vertices, std::vector <GLuint>& indices, std::vector <Texture>& textures);

	// Draws the mesh
	void DrawMesh(Shader& shader, Camera& camera);
};
#endif