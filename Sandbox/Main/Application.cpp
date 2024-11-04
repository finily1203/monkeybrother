/*All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Lew Zong Han Owen (z.lew)@team   :  MonkeHood
@course :  CSD2401@file   :  Application.cpp
@brief  :
*Lew Zong Han Owen (z.lew) :  - Integrated a try-catch block to capture custome and standard c++ exceptions for log crashing into
	external file
*Joel Chu (c.weiyuan) : Integrated the creation, update and deletion of GameSystems.
File Contributions: Lew Zong Han Owen (50%)
File Contributions: Joel Chu (50%)
/*_______________________________________________________________________________________________________________*/
#include "EngineDefinitions.h"
#include <GL/glew.h> //To include glew, must include it before glfw3.h
#include <iostream>
#include "WindowSystem.h"
#include "GlfwFunctions.h"
#include "Engine.h"
#include "ECSCoordinator.h"
#include "AudioSystem.h"
#include "GlobalCoordinator.h"
#include "Crashlog.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


namespace monkeybrother {
	__declspec(dllimport) void Print();
}

int main() {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(54393);


	try {
		CrashLog::SignalChecks();
		CrashLog::Initialise();
		Engine* engine = new Engine();

		WindowSystem* windowSystem = new WindowSystem();
		engine->addSystem(windowSystem);

		engine->addSystem(&assetsManager);

		AudioSystem* audioSystem = new AudioSystem();
		engine->addSystem(audioSystem);

		engine->addSystem(&ecsCoordinator);

		engine->addSystem(&graphicsSystem);
	
		engine->addSystem(&debugSystem);

		engine->addSystem(&fontSystem);

		engine->initialiseSystem();
		ecsCoordinator.initialiseSystemsAndComponents();
		//ecsCoordinator.test3();

		while (!glfwWindowShouldClose(GLFWFunctions::pWindow)) {
			//DebugSystem::StartLoop(); //Get time for start of gameloop

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
		_CrtDumpMemoryLeaks();
	}
	return 0;
}
