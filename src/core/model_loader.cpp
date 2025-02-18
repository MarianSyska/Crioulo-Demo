#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <stb_image/stb_image.h>

#include "model_loader.h"

Model ModelLoader::loadModel(std::string const& path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    Model model;
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
    }

    std::vector<std::string> loadedTexturesPaths;
    std::string directory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene, model, directory, loadedTexturesPaths);
    return model;
}

void ModelLoader::processNode(aiNode *node, const aiScene *scene, Model& model, const std::string& directory, std::vector<std::string>& loadedTexturesPaths)
{
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene, model, directory, loadedTexturesPaths);
    }
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene, model, directory, loadedTexturesPaths);
    }
}

void ModelLoader::processMesh(aiMesh *mesh, const aiScene *scene, Model& model, const std::string& directory, std::vector<std::string>& loadedTexturesPaths)
{
    model.meshes.push_back(MeshData());
    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector;
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }
        if(mesh->mTextureCoords[0])
        {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x; 
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec; 
        }
        else
        {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }
        model.meshes.back().vertices.push_back(vertex);
    }

    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            model.meshes.back().indices.push_back(face.mIndices[j]);        
    }

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", model, directory, loadedTexturesPaths);
    loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", model, directory, loadedTexturesPaths);
    loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal", model, directory, loadedTexturesPaths);
    loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_height", model, directory, loadedTexturesPaths);

}

void ModelLoader::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string name, Model& model, const std::string& directory, std::vector<std::string>& loadedTexturesPaths)
{
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);

        bool skip = false;
        for(unsigned int j = 0; j < loadedTexturesPaths.size(); j++)
        {
            if(std::strcmp(loadedTexturesPaths[j].c_str(), str.C_Str()) == 0)
            {
                skip = true;
                model.meshes.back().textureIds.push_back({name + std::to_string(i), j});
                break;
            } 
        }
        if(!skip)
        {   
            loadedTexturesPaths.push_back(std::string(str.C_Str()));
            model.textures.push_back(TextureFromFile(str.C_Str(), directory));
            model.meshes.back().textureIds.push_back({name + std::to_string(i), static_cast<unsigned int>(model.textures.size()) - 1});
        }
    }
}

TextureData ModelLoader::TextureFromFile(const char* path, const std::string& directory)
{
    std::string filename = std::string(path);
    filename = directory + '/' + filename;


    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (!data)
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return {height, width, nrComponents, data};
}

ModelLoader* ModelLoader::singleton = nullptr;