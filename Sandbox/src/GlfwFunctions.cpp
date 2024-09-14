#include "GlfwFunctions.h"
#include <iostream>

bool GLFWFunctions::isAKeyPressed = false;
GLFWwindow* GLFWFunctions::pWindow = nullptr;
double GLFWFunctions::fps = 0.0;

bool GLFWFunctions::init(int width, int height, std::string title) {

    /* Initialize the library */
    if (!glfwInit())
        return false;

    /* Create a windowed mode window and its OpenGL context */
    GLFWFunctions::pWindow = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    if (!GLFWFunctions::pWindow)
    {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        return false;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(GLFWFunctions::pWindow);
    
    callEvents();

    glfwSetInputMode(GLFWFunctions::pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    return true;
}

void GLFWFunctions::callEvents() {

    glfwSetKeyCallback(GLFWFunctions::pWindow, GLFWFunctions::keyboardEvent);
    glfwSetMouseButtonCallback(GLFWFunctions::pWindow, GLFWFunctions::mouseButtonEvent);
    glfwSetCursorPosCallback(GLFWFunctions::pWindow, GLFWFunctions::cursorPositionEvent);
    glfwSetScrollCallback(GLFWFunctions::pWindow, GLFWFunctions::scrollEvent);
}

void GLFWFunctions::keyboardEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (GLFW_PRESS == action) {
#ifdef _DEBUG
        std::cout << "Key pressed" << std::endl;
#endif
    }
    else if (GLFW_REPEAT == action) {
#ifdef _DEBUG
        std::cout << "Key repeatedly pressed" << std::endl;
#endif
    }
    else if (GLFW_RELEASE == action) {
#ifdef _DEBUG
        std::cout << "Key released" << std::endl;
#endif
    }

    if (GLFW_KEY_A == key && GLFW_PRESS == action && GLFWFunctions::isAKeyPressed == false) {
		GLFWFunctions::isAKeyPressed = true;
	} 
    else if (GLFW_KEY_A == key && GLFW_PRESS == action && GLFWFunctions::isAKeyPressed == true) {
        GLFWFunctions::isAKeyPressed = false;
    }

    if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void GLFWFunctions::mouseButtonEvent(GLFWwindow* window, int button, int action, int mods) {
    switch (button) {
    case GLFW_MOUSE_BUTTON_LEFT:
    #ifdef _DEBUG
        std::cout << "Left mouse button ";
    #endif
        break;
    case GLFW_MOUSE_BUTTON_RIGHT:
    #ifdef _DEBUG
        std::cout << "Right mouse button ";
    #endif
        break;
    }
    switch (action) {
    case GLFW_PRESS:
    #ifdef _DEBUG
        std::cout << "pressed" << std::endl;
    #endif
        break;
    case GLFW_RELEASE:
    #ifdef _DEBUG
        std::cout << "released" << std::endl;
    #endif
        break;
    }
}

void GLFWFunctions::cursorPositionEvent(GLFWwindow * window, double xpos, double ypos) {
    #ifdef _DEBUG
	std::cout << "Cursor position: " << xpos << ", " << ypos << std::endl;
    #endif
}

void GLFWFunctions::scrollEvent(GLFWwindow* window, double xoffset, double yoffset) {
    #ifdef _DEBUG
	std::cout << "Scroll offset: " << xoffset << ", " << yoffset << std::endl;
	#endif
}

void GLFWFunctions::showFPS(GLFWwindow* window) {
    double currTime = glfwGetTime();
    static double prevTime = glfwGetTime();
    double dt = currTime - prevTime;
    GLFWFunctions::fps = 1000 / dt;

 //   double count = 0.0;
 //   count++;

 //   double elapsedTime = currTime - prevTime;
 //   if (dt > 1.0) {
	//	GLFWFunctions::fps = elapsedTime / count;
	//	prevTime = currTime;
 //       count = 0.0;
	//}
}

void GLFWFunctions::glfwCleanup() {
	glfwTerminate();
}