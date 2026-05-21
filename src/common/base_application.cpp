#include <tracy/Tracy.hpp>
#include <Crioulo/renderer.h>
#include <Crioulo/init.h>
#include <iostream>
#include <GLFW/glfw3.h>
#include <memory>

#include "CriouloDemo/common/base_application.h"


WindowContext::WindowContext(GLFWwindow& window) :
    m_window(&window)
{}

void WindowContext::makeCurrent() {
    glfwMakeContextCurrent(m_window);
}

void WindowContext::doneCurrent() {
    glfwMakeContextCurrent(nullptr);
}

BaseApplication::BaseApplication(const std::string& name, int width, int height) :
    m_window(createWindow(name, width, height)),
    m_context(*m_window)
{
    // TODO: Remove Glad reference
    Crioulo::initialize((GLADloadproc)glfwGetProcAddress, m_context);

    Crioulo::RendererSettings renderer_settings;
    m_renderer = std::unique_ptr<Crioulo::Renderer>(new Crioulo::Renderer(m_context, renderer_settings));
}

BaseApplication::~BaseApplication() {
    glfwDestroyWindow(m_window);
}

int BaseApplication::run() {

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    while (!glfwWindowShouldClose(m_window))
    {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;


        glfwPollEvents();
        update(deltaTime);
         m_renderer->drawScene();
        glfwSwapBuffers(m_window);
        FrameMark;
    }
    
    glfwTerminate();
    return 0;
}

GLFWwindow* BaseApplication::createWindow(const std::string& name, int width, int height) {
    if (!isInialized) {
        glfwSetErrorCallback([](int code, const char* description) {
            std::cout << code << ": " << description << std::endl;
        });
        if (glfwInit() == GLFW_FALSE) {
            throw std::runtime_error("GLFW could not be initialized.");
        }
        isInialized = true;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);


#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        BaseApplication* self = static_cast<BaseApplication*>(glfwGetWindowUserPointer(window));
        self->resizeFramebuffer(width, height);
    });
    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xPosition, double yPosition) {
        BaseApplication* self = static_cast<BaseApplication*>(glfwGetWindowUserPointer(window));
        self->onMouseInput(xPosition, yPosition);
    });
    glfwSetScrollCallback(window, [](GLFWwindow* window, double xOffset, double yOffset) {
        BaseApplication* self = static_cast<BaseApplication*>(glfwGetWindowUserPointer(window));
        self->onMouseInput(xOffset, yOffset);
    });

    return window;
}


void BaseApplication::resizeFramebuffer(int width, int height) {
    m_renderer->adjustViewport(0, 0, width, height);
}