#ifndef CRIOULO_DEMO_APPLICATION_H
#define CRIOULO_DEMO_APPLICATION_H

#include <Crioulo/camera.h>

#include "CriouloDemo/common/base_application.h"

class Application : public BaseApplication
{
	public:
		Application(const std::string& modelPath);

		void update(float deltaTime) override;
		void onMouseInput(double xPosition, double yPosition) override;
		inline void onMouseScrollInput(double xOffset, double yOffset) override {};

	private:

		Crioulo::Camera m_camera;

		// Input
		bool m_firstMouse;
		float m_lastX;
		float m_lastY;
		float m_pitch;
		float m_yaw;

		Crioulo::PointLight* m_pointLight;

		glm::vec3 m_lightCenter;
		float m_lightRadius;
		float m_lightSpeed;
};

#endif