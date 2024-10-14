#include <GL/glew.h> //To include glew, must include it before glfw3.h
#include <iostream>
#include "WindowSystem.h"
#include "GlfwFunctions.h"
#include "Engine.h"
#include "ECSCoordinator.h"
#include "AudioSystem.h"
#include "GlobalCoordinator.h"
#include "Crashlog.h"


namespace monkeybrother {
	__declspec(dllimport) void Print();
}

int main() {
	try {
		CrashLog::SignalChecks();
		CrashLog::Initialise();
		Engine* engine = new Engine();

		WindowSystem* windowSystem = new WindowSystem();
		engine->addSystem(windowSystem);

		AudioSystem* audioSystem = new AudioSystem();
		engine->addSystem(audioSystem);

		engine->addSystem(&ecsCoordinator);

		engine->addSystem(&debugSystem);

		engine->initialiseSystem();
		//ecsCoordinator.test();
		ecsCoordinator.test2();

		while (!glfwWindowShouldClose(GLFWFunctions::pWindow)) {
			//If user presses clone button ("C"), clone first object
			if (GLFWFunctions::cloneObject) {
				ecsCoordinator.cloneEntity(ecsCoordinator.getFirstEntity());
				GLFWFunctions::cloneObject = false;
			}

			engine->updateSystem();
			glfwSwapBuffers(GLFWFunctions::pWindow);
		}


		engine->cleanupSystem();
		delete engine;

		CrashLog::Cleanup();
	}
	catch (const CrashLog::Exception& e) {
		std::cerr << "Program crashed! Check crash-log.txt for more information" << std::endl;
		CrashLog::LogDebugMessage(e.message, e.file, e.line);
		CrashLog::LogDebugMessage("End Log");
	}
	catch (const std::exception& e) {
		std::cerr << "Program crashed! Check crash-log.txt for more information" << e.what() << std::endl;
		CrashLog::LogDebugMessage(e.what());
		CrashLog::LogDebugMessage("End Log");
	}
	catch (...) {
		std::cerr << "Program crashed! Check crash-log.txt for more information" << std::endl;
		CrashLog::LogDebugMessage("Unknown exception caught");
		CrashLog::LogDebugMessage("End Log");
	}
	
	return 0;
}
