#include <array>

#include "CriouloDemo/examples/01_simple_squad/application.h"
#include "CriouloDemo/util/util.h"
#include "CriouloDemo/util/filesystem.h"


struct SimpleVertex {
    glm::vec2 position;

    static const std::array<Crioulo::VertexAttributeDetails, 1> attributes;
};

const std::array<Crioulo::VertexAttributeDetails, 1> SimpleVertex::attributes = {
    {0, 2, GL_FLOAT, GL_FALSE, sizeof(SimpleVertex), (void*)offsetof(SimpleVertex, position)}
};

Application::Application() :
    BaseApplication("Simple Quad", 600, 600) 
{
    std::string vertexShader;
    std::string fragmentShader;

    try
    {
        vertexShader = loadFile(FileSystem::getPath("shaders/examples/01_simple_squad/simple_vs.glsl").c_str());
        fragmentShader = loadFile(FileSystem::getPath("shaders/examples/01_simple_squad/simple_fs.glsl").c_str());
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }

    std::shared_ptr<Crioulo::Shader> shader = m_renderer->loadShader(vertexShader.c_str(), fragmentShader.c_str());


    Crioulo::MeshData<SimpleVertex> meshData;
    meshData.vertices = {
        { { -1.0f, 1.0f } },
        { { -1.0f, -1.0f } },
        { { 1.0f, -1.0f } },
        { { 1.0f,  1.0f } }
    };
    meshData.indices = { 0, 1, 2, 0, 2, 3 };
    std::shared_ptr<Crioulo::Mesh> mesh = m_renderer->loadMesh(meshData);


    std::vector<Crioulo::TextureSlot> emptyTextures;
    Crioulo::Material material(shader, emptyTextures);
    Crioulo::MeshInstance* instance = m_renderer->createMeshInstance(mesh, material);
}