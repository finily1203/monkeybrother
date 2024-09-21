#pragma once
#include "GlfwFunctions.h"
#include "Systems.h"

//create class for windows application
class WindowSystem : public Systems
{
public:
	WindowSystem() = default;
	~WindowSystem() = default;

	//initialise, update and cleanup function for window class 
	//override the virtual function for systems
	void initialise() override;
	void update() override;
	void cleanup() override;
};
