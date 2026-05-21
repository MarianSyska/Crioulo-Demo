#ifndef CRIOULO_DEMO_BASE_APPLICATION_H
#define CRIOULO_DEMO_BASE_APPLICATION_H

#include <Crioulo/global_context_manager.h>
#include <Crioulo/renderer.h>

class GLFWwindow;

class WindowContext : public Crioulo::IContext {
public:
    WindowContext(GLFWwindow& window);

    void makeCurrent() override;

    void doneCurrent() override;

private:
    GLFWwindow* m_window;

};

class BaseApplication {
	public:
        BaseApplication(const std::string& name, int width, int height);
        BaseApplication(const BaseApplication& other) = delete;
		BaseApplication(BaseApplication&& other) = delete;
		virtual ~BaseApplication();

		BaseApplication* operator=(const BaseApplication& other) = delete;
		BaseApplication* operator=(BaseApplication&& other) = delete;

        int run();

    protected:
        virtual void update(float deltaTime) = 0;

        virtual void onMouseInput(double xPosition, double yPosition) = 0;
        virtual void onMouseScrollInput(double xOffset, double yOffset) = 0;

        GLFWwindow* m_window;
        WindowContext m_context;
        std::unique_ptr<Crioulo::Renderer> m_renderer;

    private:
        inline static bool isInialized = false;

        GLFWwindow* createWindow(const std::string& name, int width, int height);

        void resizeFramebuffer(int width, int height);
};


#endif