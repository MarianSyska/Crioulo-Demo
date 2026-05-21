#ifndef SIMPLE_QUAD_EXAMPLE_APPLICATION_H
#define SIMPLE_QUAD_EXAMPLE_APPLICATION_H

#include <CriouloDemo/common/base_application.h>

class Application : public BaseApplication
{
	public:
		Application();

	protected:
		inline void update(float deltaTime) override {}

		inline void onMouseInput(double xPosition, double yPosition) override {};
		inline void onMouseScrollInput(double xOffset, double yOffset) override {};
};

#endif