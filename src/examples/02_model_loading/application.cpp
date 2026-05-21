#define GLFW_INCLUDE_NONE 
#include <tracy/Tracy.hpp>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "CriouloDemo/examples/02_model_loading/application.h"
#include "CriouloDemo/util/util.h"
#include "CriouloDemo/util/filesystem.h"
#include "CriouloDemo/common/model_loader.h"

namespace {
    constexpr std::array<const char*, 6> skyBoxFiles = {
        "data/textures/skybox/right.jpg",
        "data/textures/skybox/left.jpg",
        "data/textures/skybox/top.jpg",
        "data/textures/skybox/bottom.jpg",
        "data/textures/skybox/front.jpg",
        "data/textures/skybox/back.jpg"
    };
}

Application::Application(const std::string& modelPath) :
    BaseApplication("Model loading", 600, 600),
    m_yaw(0.0f),
    m_pitch(0.0f),
    m_lightCenter(),
    m_lightRadius(10.0f),
    m_lightSpeed(1.0f)
{
    stbi_set_flip_vertically_on_load(true);

    ModelLoader* loader = ModelLoader::getInstance();
    Model model = loader->loadModel(modelPath);

    // Load Textures
    std::vector<std::shared_ptr<Crioulo::Texture>> textures;

    for (const TextureData& loaderTextureData : model.textures)
    {
        Crioulo::Texture2DData textureData{};
        textureData.height = loaderTextureData.height;
        textureData.width = loaderTextureData.width;
        textureData.nrComponents = loaderTextureData.nrComponents;
        textureData.data = loaderTextureData.data;
        textures.push_back(m_renderer->loadTexture(textureData));
    }


    // Load Shader
    std::string vertexShader;
    std::string fragmentShader;

    try
    {
        vertexShader = loadFile(FileSystem::getPath("shaders/examples/02_model_loading/model_loading_vs.glsl").c_str());
        fragmentShader = loadFile(FileSystem::getPath("shaders/examples/02_model_loading/model_loading_fs.glsl").c_str());
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }

    std::shared_ptr<Crioulo::Shader> shader = m_renderer->loadShader(vertexShader.c_str(), fragmentShader.c_str());


    // Create MeshInstances
    std::vector<Crioulo::MeshInstance*> instances;
    for (const MeshData& loaderMeshData : model.meshes)
    {
        Crioulo::MeshData<Vertex> meshData;
        meshData.vertices = loaderMeshData.vertices;
        meshData.indices = loaderMeshData.indices;

        std::shared_ptr<Crioulo::Mesh> mesh = m_renderer->loadMesh(meshData);


        std::vector<Crioulo::TextureSlot> instanceTextures;
        for (const TextureID& textureId : loaderMeshData.textureIds)
        {
            instanceTextures.push_back({ textures.at(textureId.index), textureId.name });
        }
        Crioulo::Material instanceMaterial(shader, instanceTextures);


        Crioulo::MeshInstance* instance = m_renderer->createMeshInstance(mesh, instanceMaterial);


        glm::mat4 instanceTransform = glm::mat4(1.0f);
        instanceTransform = glm::translate(instanceTransform, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        instanceTransform = glm::scale(instanceTransform, glm::vec3(1.0f, 1.0f, 1.0f));
        instance->setTransform(instanceTransform);
    }


    // Set Camera
    m_camera.position = glm::vec3(0.0f, 0.0f, 3.0f);
    m_camera.up = glm::vec3(glm::vec3(0.0f, 1.0f, 0.0f));
    m_camera.front = glm::vec3(0.0f, 0.0f, -1.0f);
    m_camera.right = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    m_camera.zoom = 45.0f;
    m_camera.nearPlane = 0.1f;
    m_camera.farPlane = 100.0f;

    m_renderer->setCamera(m_camera);


    // Set PointLights
    m_pointLight = m_renderer->createPointLight();

    m_pointLight->color = glm::vec3(1.0f, 1.0f, 1.0f);
    m_pointLight->position = m_lightCenter + m_lightRadius * glm::vec3(1.0f, 0.0f, 0.0f);
    m_pointLight->intensity = 200.0f;

    // Set SkyBox
    Crioulo::CubeMapTextureData skyBox{};
    int i = 0;
    for (auto filePath : skyBoxFiles) {
        std::string filename = FileSystem::getPath(filePath);
        TextureData textureData = loadTextureFromFile(filename);
        skyBox.height[i] = textureData.height;
        skyBox.width[i] = textureData.width;
        skyBox.nrComponents[i] = textureData.nrComponents;
        skyBox.data[i] = textureData.data;
        i++;
    }
    std::shared_ptr<Crioulo::Texture> skyBoxTexture = m_renderer->loadTexture(skyBox);
    
    m_renderer->setSkyBox(skyBoxTexture);
}

void Application::onMouseInput(double xPosition, double yPosition) {
    /*if (m_firstMouse)
    {
        m_lastX = static_cast<float>(xPosition);
        m_lastY = static_cast<float>(yPosition);
        m_firstMouse = false;
    }

    float xoffset = static_cast<float>(xPosition) - m_lastX;
    float yoffset = m_lastY - static_cast<float>(yPosition);
    m_lastX = static_cast<float>(xPosition);
    m_lastY = static_cast<float>(yPosition);

    float sensitivity = 0.00001f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    m_yaw += xoffset;
    m_pitch += yoffset;

    if (m_pitch > 89.0f)
        m_pitch = 89.0f;
    if (m_pitch < -89.0f)
        m_pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    direction.y = sin(glm::radians(m_pitch));
    direction.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

    m_camera.front = glm::normalize(direction);
    m_camera.right = glm::normalize(glm::cross(m_camera.front, glm::vec3(0.0f, 1.0f, 0.0f)));
    m_camera.up = glm::normalize(glm::cross(m_camera.right, m_camera.front));*/
}

void Application::update(float deltaTime) {
    ZoneScoped;
    const float cameraSpeed = 2.0f * deltaTime;
    if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
        m_camera.position += cameraSpeed * m_camera.front;
    if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
        m_camera.position -= cameraSpeed * m_camera.front;
    if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
        m_camera.position -= glm::normalize(glm::cross(m_camera.front, m_camera.up)) * cameraSpeed;
    if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
        m_camera.position += glm::normalize(glm::cross(m_camera.front, m_camera.up)) * cameraSpeed;
    if (glfwGetKey(m_window, GLFW_KEY_Q) == GLFW_PRESS)
        m_camera.position += glm::vec3(0.0f, 1.0f, 0.0f) * cameraSpeed;
    if (glfwGetKey(m_window, GLFW_KEY_E) == GLFW_PRESS)
        m_camera.position -= glm::vec3(0.0f, 1.0f, 0.0f) * cameraSpeed;
    if (glfwGetKey(m_window, GLFW_KEY_UP) == GLFW_PRESS)
        m_pitch += cameraSpeed * 20.0f;
    if (glfwGetKey(m_window, GLFW_KEY_DOWN) == GLFW_PRESS)
        m_pitch -= cameraSpeed * 20.0f;
    if (glfwGetKey(m_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        m_yaw += cameraSpeed * 20.0f;
    if (glfwGetKey(m_window, GLFW_KEY_LEFT) == GLFW_PRESS)
        m_yaw -= cameraSpeed * 20.0f;

    glm::vec3 direction{};
    direction.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    direction.y = sin(glm::radians(m_pitch));
    direction.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

    m_camera.front = glm::normalize(direction);
    m_camera.right = glm::normalize(glm::cross(m_camera.front, glm::vec3(0.0f, 1.0f, 0.0f)));
    m_camera.up = glm::normalize(glm::cross(m_camera.right, m_camera.front));

    m_renderer->setCamera(m_camera);

    
    //m_pointLight->position *= glm::rotate(glm::mat4(1.0f), m_lightSpeed * deltaTime, glm::vec3(0.0f, 1.0f, 0.0f));
}