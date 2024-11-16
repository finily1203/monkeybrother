/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   WindowSystem.h
@brief:  This header file includes the class WindowSystem that handles the window
		 application for the game engine. It will initialise, update and cleanup the
		 window system.
		 Joel Chu (c.weiyuan): Declared the class WindowSystem and its functions.
							   100%
*//*___________________________________________________________________________-*/
#pragma once
#include "EngineDefinitions.h"
#include "GlfwFunctions.h"
#include "Systems.h"
#include <Shader.h>
#include "../Nlohmann/json.hpp"
#include "../FilePaths/filePath.h"
#include <fstream>

//create class for windows application
class WindowSystem : public GameSystems
{
public:
	WindowSystem() = default;
	~WindowSystem() = default;

	//initialise, update and cleanup function for window class 
	//override the virtual function for systems
	void initialise() override;
	void update() override;
	void cleanup() override;
	SystemType getSystem() override; //For perfomance viewer
	void handleWindowFocus();

private:
	bool isInterrupted = false;
	bool wasFocused = true;

	//void logicUpdate();
	
};
