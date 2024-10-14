#pragma once
#include "GlfwFunctions.h"
#include "Systems.h"
#include <Shader.h>
#include "../Nlohmann/json.hpp"
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

	void logicUpdate();
	
};
