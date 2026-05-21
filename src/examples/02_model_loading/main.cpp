#include "CriouloDemo/util/filesystem.h"
#include "CriouloDemo/examples/02_model_loading/application.h"

int main() {
	Application app(FileSystem::getPath("data/models/backpack/backpack.obj"));
	return app.run();
}