#include <GL/glew.h> //To include glew, must include it before glfw3.h
#include <iostream>
#include "WindowSystem.h"
#include "Debug.h"
#include "GlfwFunctions.h"
#include "Engine.h"
#include "ECSCoordinator.h"
#include "AudioSystem.h"


namespace monkeybrother {
	__declspec(dllimport) void Print();
}

int main() {
	monkeybrother::Print();
	Engine* engine = new Engine();
	ECSCoordinator* ecsCoordinator = new ECSCoordinator();
	ecsCoordinator->test();


	WindowSystem* windowSystem = new WindowSystem();
	engine->addSystem(windowSystem);

	AudioSystem* audioSystem = new AudioSystem();
	engine->addSystem(audioSystem);
	
	engine->initialiseSystem();
	while (!glfwWindowShouldClose(GLFWFunctions::pWindow)) {
		engine->updateSystem();
	}

	engine->cleanupSystem();

	delete engine;

	return 0;
}