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
	static double delta_time;
};

#endif
//END HEADER GUARDS//