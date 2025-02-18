#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <string>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <stb_image/stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

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

struct TextureData
{
    int height;
    int width;
    int nrComponents;
    unsigned char* data;
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

        static ModelLoader* get()
        {
            if(!singleton)
            {
                singleton = new ModelLoader();
                stbi_set_flip_vertically_on_load(true);
            }
            return singleton;
        }

    private:

        static ModelLoader* singleton;

        void processNode(aiNode *node, const aiScene *scene, Model &model, const std::string &directory, std::vector<std::string>& loadedTexturesPaths);
        void processMesh(aiMesh *mesh, const aiScene *scene, Model &model, const std::string &directory, std::vector<std::string>& loadedTexturesPaths);
        void loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string name, Model &model, const std::string &directory, std::vector<std::string>& loadedTexturesPaths);
        TextureData TextureFromFile(const char *path, const std::string& directory);
        ModelLoader(){}
};
#endif