#define STB_IMAGE_IMPLEMENTATION

#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>

#include "core/renderer/renderer.h"
#include "core/model_loader.h"
#include "core/filesystem.h"

void processInput(GLFWwindow *window);
GLFWwindow* createWindow();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void error_callback(int code, const char* description);
std::string loadFile(const char* filename);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f;

float yaw;
float pitch;

glm::vec3 cameraPosition;
glm::vec3 cameraFront;
glm::vec3 cameraUp;
glm::vec3 cameraRight;

std::shared_ptr<Crioulo::Camera> camera = std::make_shared<Crioulo::Camera>();

int main()
{
    {
        GLFWwindow* window = createWindow();

        Crioulo::Renderer renderer(window);

        ModelLoader* loader = ModelLoader::get();
        Model model = loader->loadModel(FileSystem::getPath("data/models/backpack/backpack.obj"));

        std::vector<std::shared_ptr<Crioulo::Texture>> textures;

        for(const TextureData& loaderTextureData : model.textures)
        {
            Crioulo::TextureData textureData;
            textureData.height = loaderTextureData.height;
            textureData.width = loaderTextureData.width;
            textureData.nrComponents = loaderTextureData.nrComponents;
            textureData.data = loaderTextureData.data;
            textures.push_back(renderer.loadTexture(textureData));
        }

        std::string vertexShader;
        std::string fragmentShader;

        try
        {
            vertexShader = loadFile(FileSystem::getPath("data/shaders/model_loading_vs.glsl").c_str());
            fragmentShader = loadFile(FileSystem::getPath("data/shaders/model_loading_fs.glsl").c_str());
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        }

        std::shared_ptr<Crioulo::Shader> shader = renderer.loadShader(vertexShader.c_str(), fragmentShader.c_str());

        std::vector<std::shared_ptr<Crioulo::MeshInstance>> instances(model.meshes.size());
        
        for(const MeshData& loaderMeshData : model.meshes)
        {  
            Crioulo::MeshData meshData;
            for(const Vertex& loaderVertexData : loaderMeshData.vertices)
            {
                Crioulo::Vertex vertexData;
                vertexData.Position = loaderVertexData.Position;
                vertexData.Normal = loaderVertexData.Normal;
                vertexData.TexCoords = loaderVertexData.TexCoords;
                meshData.vertices.push_back(vertexData);
            }
            meshData.indices = loaderMeshData.indices;
            std::shared_ptr<Crioulo::Mesh> mesh = renderer.loadMesh(meshData);

            std::vector<Crioulo::TextureSlot> instanceTextures;

            for(const TextureID& textureId : loaderMeshData.textureIds)
            {
                instanceTextures.push_back({textures.at(textureId.index), textureId.name});
            }

            std::shared_ptr<Crioulo::Material> instanceMaterial = std::make_shared<Crioulo::Material>(shader, instanceTextures);

            std::shared_ptr<Crioulo::MeshInstance> instance = std::make_shared<Crioulo::MeshInstance>(mesh, instanceMaterial);

            instances.push_back(instance);

            glm::mat4 instanceTransform = glm::mat4(1.0f);
            instanceTransform = glm::translate(instanceTransform, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
            instanceTransform = glm::scale(instanceTransform, glm::vec3(1.0f, 1.0f, 1.0f));	
            instance->setTransform(instanceTransform);

            renderer.registerMeshInstance(instance);
        }

        cameraPosition = glm::vec3(0.0f, 0.0f,  3.0f);
        cameraUp = glm::vec3(glm::vec3(0.0f, 1.0f,  0.0f));
        cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
        cameraRight = glm::cross(glm::vec3(0.0f, 1.0f,  0.0f), glm::vec3(0.0f, 0.0f, -1.0f));

        camera->setPosition(cameraPosition);
        camera->setUp(cameraUp);
        camera->setFront(cameraFront);
        camera->setRight(cameraRight);
        camera->setZoom(45.0f);
        camera->setNearPlane(0.1f);
        camera->setFarPlane(100.0f);

        renderer.setActiveCamera(camera);

        std::shared_ptr<Crioulo::PointLight> pointLight = std::make_shared<Crioulo::PointLight>();

        pointLight->setColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
        pointLight->setPosition(glm::vec3(1.0f, 0.0f, 0.0f));
        pointLight->setIntensity(0.5f);

        renderer.registerPointLight(pointLight);

        while(!glfwWindowShouldClose(window))
        {
            float currentFrame = (float) glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;


            glfwPollEvents();
            processInput(window);
            renderer.drawScene();
        }
    }
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    const float cameraSpeed = 0.5f * deltaTime; // adjust accordingly
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPosition +=  cameraSpeed * cameraFront;
        camera->setPosition(cameraPosition);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPosition -= cameraSpeed * cameraFront;
        camera->setPosition(cameraPosition);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        camera->setPosition(cameraPosition);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        camera->setPosition(cameraPosition);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        cameraPosition += glm::vec3(0.0f, 1.0f, 0.0f) * cameraSpeed;
        camera->setPosition(cameraPosition);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        cameraPosition -= glm::vec3(0.0f, 1.0f, 0.0f) * cameraSpeed;
        camera->setPosition(cameraPosition);
}

GLFWwindow* createWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    glfwSetErrorCallback(error_callback);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    return window;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = (float) xpos;
        lastY = (float) ypos;
        firstMouse = false;
    }
    
    float xoffset = ((float) xpos) - lastX;
    float yoffset = lastY - ((float) ypos); 
    lastX = (float) xpos;
    lastY = (float) ypos;

    float sensitivity = 0.00001f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    
    cameraFront = glm::normalize(direction);
    cameraRight = glm::normalize(glm::cross(cameraFront, glm::vec3(0.0f, 1.0f, 0.0f))); 
    cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));

    camera->setFront(cameraFront);
    camera->setRight(cameraRight);
    camera->setUp(cameraUp);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    //camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void error_callback(int code, const char* description)
{
    std::cout << code << ": " << description << std::endl;
}

std::string loadFile(const char* filename)
{
    std::string content;
    std::ifstream file;
    std::stringstream strStream;

    file.exceptions (std::ifstream::failbit | std::ifstream::badbit);

    file.open(filename);
    strStream << file.rdbuf();	
    file.close();

    content = strStream.str();

    return content;
}