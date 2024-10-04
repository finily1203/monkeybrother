/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan), Yaoting (yaoting.liu)
@team:   MonkeHood
@course: CSD2401
@file:   GLFWFunctions.h
@brief:  This header file includes the functions to hand GLFW events and callbacks.
		 Joel Chu (c.weiyuan): Declared all of the functions and most of the variables
							   80%
		 Yaoting (yaoting.liu): Declared variables for movement flags and debug flag.
							   20%
*//*___________________________________________________________________________-*/

//HEADER GUARDS//
#ifndef GLFW_FUNCTIONS_H
#define GLFW_FUNCTIONS_H
//#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>

struct GLFWFunctions {
	
	// Initialize the window
	static bool init(int width, int height, std::string title);

	//Handle keyboard events
	static void keyboardEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
	//Handle mouse button events
	static void mouseButtonEvent(GLFWwindow* window, int button, int action, int mods);
	//Handle cursor position events
	static void cursorPositionEvent(GLFWwindow* window, double xpos, double ypos);
	//Handle scroll events
	static void scrollEvent(GLFWwindow* window, double xoffset, double yoffset);
	//Handle window to check for events
	static void callEvents();

	//terminates the window
	static void glfwCleanup();

	//Caluclate the FPS and delta_time to be used
	static void getFps(double fpsCalculateInt);

	static bool isAKeyPressed;
	static GLFWwindow* pWindow;
	static double fps;
	static float delta_time;
	static float volume;
	static bool audioPaused;
	static bool nextSong;
	static bool audioStopped;
	static bool cloneObject;
	static bool goNextMode;

	static GLboolean left_turn_flag;
	static GLboolean right_turn_flag;
	static GLboolean scale_up_flag;
	static GLboolean scale_down_flag;
	static GLboolean move_up_flag;
	static GLboolean move_down_flag;
	static GLboolean move_left_flag;
	static GLboolean move_right_flag;
	static GLboolean debug_flag;
	static GLboolean move_jump_flag;

	static bool isGuiOpen;

	static int testMode;
};

#endif
//END HEADER GUARDS//