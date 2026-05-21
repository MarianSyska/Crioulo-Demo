#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>

#include "CriouloDemo/common/model_loader.h"

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
        vertex.position = vector;
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.normal = vector;
        }
        if(mesh->mTextureCoords[0])
        {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x; 
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.texCoords = vec; 
        }
        else
        {
            vertex.texCoords = glm::vec2(0.0f, 0.0f);
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

    for (unsigned int type = aiTextureType_NONE; type <= aiTextureType_UNKNOWN; ++type) {
        unsigned int count = material->GetTextureCount((aiTextureType)type);
        for (unsigned int i = 0; i < count; ++i) {
            aiString path;
            material->GetTexture((aiTextureType)type, i, &path);

            printf("Slot: %d | Index: %d | Path: %s\n",
                type, i, path.C_Str());
        }
    }

    loadMaterialTextures(material, aiTextureType_DIFFUSE, "uAlbedoMap", model, directory, loadedTexturesPaths);
    loadMaterialTextures(material, aiTextureType_SPECULAR, "uMetalnessMap", model, directory, loadedTexturesPaths);
    loadMaterialTextures(material, aiTextureType_SHININESS, "uRoughnessMap", model, directory, loadedTexturesPaths);
    loadMaterialTextures(material, aiTextureType_AMBIENT, "uAOMap", model, directory, loadedTexturesPaths);
    loadMaterialTextures(material, aiTextureType_HEIGHT, "uNormalMap", model, directory, loadedTexturesPaths);

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
            std::string filename = directory + '/' + std::string(str.C_Str());
            loadedTexturesPaths.push_back(std::string(str.C_Str()));
            model.textures.push_back(loadTextureFromFile(filename));
            model.meshes.back().textureIds.push_back({name + std::to_string(i), static_cast<unsigned int>(model.textures.size()) - 1});
        }
    }
}

