#pragma once
#include "GlfwFunctions.h"
#include "Systems.h"
#include <Shader.h>

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

	GLboolean left_turn_flag;
	GLboolean right_turn_flag;
	GLboolean scale_up_flag;
	GLboolean scale_down_flag;
	GLboolean move_up_flag;
	GLboolean move_down_flag;
	GLboolean move_left_flag;
	GLboolean move_right_flag;

	void keyboardInputUpdateFlag();
	void logicUpdate();
	
};
