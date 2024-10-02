//HEADER GUARDS//
#ifndef GLFW_FUNCTIONS_H
#define GLFW_FUNCTIONS_H
//#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>

struct GLFWFunctions {
	static bool init(int width, int height, std::string title);


	static void keyboardEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouseButtonEvent(GLFWwindow* window, int button, int action, int mods);
	static void cursorPositionEvent(GLFWwindow* window, double xpos, double ypos);
	static void scrollEvent(GLFWwindow* window, double xoffset, double yoffset);
	static void callEvents();

	static void glfwCleanup();

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

	static GLboolean left_turn_flag;
	static GLboolean right_turn_flag;
	static GLboolean scale_up_flag;
	static GLboolean scale_down_flag;
	static GLboolean move_up_flag;
	static GLboolean move_down_flag;
	static GLboolean move_left_flag;
	static GLboolean move_right_flag;
	static GLboolean debug_flag;

	static bool isGuiOpen;
};

#endif
//END HEADER GUARDS//