#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include"ShaderClass.h"
#include"Mesh.h"
#include<vector>

class Model
{
public:
    Model(std::string path)
    {
        loadModel(path);
    }
    void Draw(Shader &shader, Camera &camera);

private:
    // model data
    std::vector<Mesh> meshes;
    std::string directory;

    void loadModel(std::string const& path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};