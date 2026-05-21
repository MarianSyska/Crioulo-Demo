#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <string>
#include <array>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <stb_image/stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <Crioulo/mesh.h>
#include <CriouloDemo/util/util.h>


struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoords;

        static const std::array<Crioulo::VertexAttributeDetails, 3> attributes;
};

inline const std::array<Crioulo::VertexAttributeDetails, 3> Vertex::attributes = {{
    {0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position)},
    {1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal)},
    {2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords)}
}};

struct TextureID
{
    std::string name;
    unsigned int index;
};

struct MeshData
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<TextureID> textureIds;
};

struct Model
{
    std::vector<MeshData> meshes;
    std::vector<TextureData> textures; 
};

class ModelLoader
{
    public:

        Model loadModel(std::string const& path);

        static ModelLoader* getInstance()
        {
            static ModelLoader singleton;
            //stbi_set_flip_vertically_on_load(true);
            return &singleton;
        }

    private:

        

        void processNode(
            aiNode *node, 
            const aiScene *scene, 
            Model &model, 
            const std::string &directory, 
            std::vector<std::string>& loadedTexturesPaths);
        void processMesh(aiMesh *mesh, const aiScene *scene, Model &model, const std::string &directory, std::vector<std::string>& loadedTexturesPaths);
        void loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string name, Model &model, const std::string &directory, std::vector<std::string>& loadedTexturesPaths);
        ModelLoader(){}
};
#endif