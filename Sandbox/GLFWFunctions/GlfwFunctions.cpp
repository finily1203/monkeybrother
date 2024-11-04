/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan), Yaoting (yaoting.liu)
@team:   MonkeHood
@course: CSD2401
@file:   GLFWFunctions.cpp
@brief:  This source file includes the functions to hand GLFW events and callbacks.
		 These functions includes the creation of the window appliction, event
         calling and setting of FPS and delta time to be used by the game engine.
         Joel Chu (c.weiyuan): Defined functions to create window, call events and
                               handle FPS and delta time.
                               80%
         Yaoting (yaoting.liu): Added in variables for movement flags and debug flag.
							   20%
*//*___________________________________________________________________________-*/
#include "EngineDefinitions.h"
#include "Debug.h"
#include "Crashlog.h"
#include "GlfwFunctions.h"
#include "GUIGameViewport.h"
#include <algorithm>
#include <iostream>
//#include <chrono>
//#include <thread>

bool GLFWFunctions::isAKeyPressed = false;
GLFWwindow* GLFWFunctions::pWindow = nullptr;
double GLFWFunctions::fps = 0.0;
float GLFWFunctions::delta_time = 0.0;
float GLFWFunctions::volume = 0.5f;
float GLFWFunctions::zoomMouseCoordX = 0.0f;
float GLFWFunctions::zoomMouseCoordY = 0.0f;
float GLFWFunctions::objMoveMouseCoordX = 0.0f;
float GLFWFunctions::objMoveMouseCoordY = 0.0f;
bool GLFWFunctions::audioPaused = false;
bool GLFWFunctions::audioNext = false;
int GLFWFunctions::audioNum = 0;
int GLFWFunctions::windowWidth = 0;
int GLFWFunctions::windowHeight = 0;
bool GLFWFunctions::audioStopped = false;
bool GLFWFunctions::isGuiOpen = false;
bool GLFWFunctions::zoomViewport = false;
bool GLFWFunctions::isAtMaxZoom = false;
bool GLFWFunctions::cloneObject = false;
bool GLFWFunctions::goNextMode = false;
bool GLFWFunctions::bumpAudio = false;
bool GLFWFunctions::hasBumped = false;
bool GLFWFunctions::slideAudio = false;
bool GLFWFunctions::bubblePopping = false;

bool GLFWFunctions::enemyMoveLeft = false;
bool GLFWFunctions::enemyMoveRight = false;
bool GLFWFunctions::enemyMoveUp = false;
bool GLFWFunctions::enemyMoveDown = false;

GLboolean GLFWFunctions::left_turn_flag = false;
GLboolean GLFWFunctions::right_turn_flag = false;
GLboolean GLFWFunctions::scale_up_flag = false;
GLboolean GLFWFunctions::scale_down_flag = false;
GLboolean GLFWFunctions::move_up_flag = false;
GLboolean GLFWFunctions::move_down_flag = false;
GLboolean GLFWFunctions::move_left_flag = false;
GLboolean GLFWFunctions::move_right_flag = false;
GLboolean GLFWFunctions::debug_flag = false;
GLboolean GLFWFunctions::move_jump_flag = false;
GLboolean GLFWFunctions::allow_camera_movement = false;
GLboolean GLFWFunctions::camera_zoom_in_flag = false;
GLboolean GLFWFunctions::camera_zoom_out_flag = false;
GLboolean GLFWFunctions::camera_rotate_left_flag = false;
GLboolean GLFWFunctions::camera_rotate_right_flag = false;

int GLFWFunctions::testMode = 0;

// Initialize the window
bool GLFWFunctions::init(int width, int height, std::string title) {

    /* Initialize the library */
    if (!glfwInit())
        return false;

	// Set the window width and height
	windowWidth = width;
	windowHeight = height;

    /* Create a windowed mode window and its OpenGL context */
    GLFWFunctions::pWindow = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    if (!GLFWFunctions::pWindow)
    {
        glfwTerminate();
        //return false;
        throw CrashLog::Exception("Failed to create window", __FILE__, __LINE__);
        //std::cerr << "Failed to create window" << std::endl;
    }
    /* Make the window's context current */
    glfwMakeContextCurrent(GLFWFunctions::pWindow);
    glfwSwapInterval(0); //vsync
    callEvents();

    glfwSetInputMode(GLFWFunctions::pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    return true;
}

//Handle window to check for events
void GLFWFunctions::callEvents() {

    glfwSetKeyCallback(GLFWFunctions::pWindow, GLFWFunctions::keyboardEvent);
    //glfwSetKeyCallback(GLFWFunctions::pWindow, DebugSystem::key_callback);
    glfwSetMouseButtonCallback(GLFWFunctions::pWindow, GLFWFunctions::mouseButtonEvent);
    glfwSetCursorPosCallback(GLFWFunctions::pWindow, GLFWFunctions::cursorPositionEvent);
    glfwSetScrollCallback(GLFWFunctions::pWindow, GLFWFunctions::scrollEvent);
}

//Handle keyboard events
void GLFWFunctions::keyboardEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (GLFW_PRESS == action) {
#ifdef _DEBUG
        std::cout << "Key pressed" << std::endl;
#endif
		if (GLFW_KEY_F2 == key)
		{
            GLFWFunctions::allow_camera_movement = !GLFWFunctions::allow_camera_movement;
		}
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

    if (GLFW_KEY_F1 == key && GLFW_PRESS == action && GLFWFunctions::isGuiOpen == false) {
        GLFWFunctions::isGuiOpen = true;
        GLFWFunctions::debug_flag = true;
    }
    else if (GLFW_KEY_F1 == key && GLFW_PRESS == action && GLFWFunctions::isGuiOpen == true) {
        GLFWFunctions::isGuiOpen = false;
        GLFWFunctions::debug_flag = false;
    }

    if (GLFW_MOUSE_BUTTON_LEFT == key && GLFW_PRESS == action && GLFWFunctions::isAKeyPressed == false) {
        GLFWFunctions::isAKeyPressed = true;
    }
    else if (GLFW_KEY_A == key && GLFW_PRESS == action && GLFWFunctions::isAKeyPressed == true) {
        GLFWFunctions::isAKeyPressed = false;
    }

    if (GLFW_KEY_P == key && GLFW_PRESS == action && GLFWFunctions::audioPaused == false) {
        GLFWFunctions::audioPaused = true;
    }
    else if (GLFW_KEY_P == key && GLFW_PRESS == action && GLFWFunctions::audioPaused == true) {
        GLFWFunctions::audioPaused = false;
    }

    if (GLFW_KEY_N == key && GLFW_PRESS == action && GLFWFunctions::audioNext == false) {
        GLFWFunctions::audioNext = true;
        GLFWFunctions::audioNum = (GLFWFunctions::audioNum + 1) % 2;
    }

    if (GLFW_KEY_O == key && GLFW_PRESS == action && GLFWFunctions::audioStopped == false) {
        GLFWFunctions::audioStopped = true;
    }
    else if (GLFW_KEY_O == key && GLFW_PRESS == action && GLFWFunctions::audioStopped == true) {
        GLFWFunctions::audioStopped = false;
    }

    if (GLFW_KEY_COMMA == key && GLFW_PRESS == action) {
        volume = std::max(0.1f, volume - 0.1f);
        std::cout << "Volume: " << volume << std::endl;
    }

    if (GLFW_KEY_PERIOD == key && GLFW_PRESS == action) {
        volume = std::min(1.f, volume + 0.1f);
        std::cout << "Volume: " << volume << std::endl;
    }

    if (GLFW_KEY_C == key && GLFW_PRESS == action && GLFWFunctions::cloneObject == false) {
        GLFWFunctions::cloneObject = true;
    }

    if (GLFW_KEY_M == key && GLFW_PRESS == action) {
        GLFWFunctions::testMode = (GLFWFunctions::testMode + 1) % 2;
        GLFWFunctions::goNextMode = true;
    }

    if (GLFW_KEY_0 == key && GLFW_PRESS == action) {
        bubblePopping = true;
    }

    if (GLFW_KEY_J == key && GLFW_PRESS == action) {
		enemyMoveLeft = true;
	}
    if (GLFW_KEY_J == key && GLFW_RELEASE == action) {
		enemyMoveLeft = false;
	}

    if (GLFW_KEY_L == key && GLFW_PRESS == action) {
        enemyMoveRight = true;
    }
    if (GLFW_KEY_L == key && GLFW_RELEASE == action) {
        enemyMoveRight = false;
    }

    if (GLFW_KEY_I == key && GLFW_PRESS == action) {
        enemyMoveUp = true;
    }
    if (GLFW_KEY_I == key && GLFW_RELEASE == action) {
        enemyMoveUp = false;
    }

    if (GLFW_KEY_K == key && GLFW_PRESS == action) {
        enemyMoveDown = true;
    }
    if (GLFW_KEY_K == key && GLFW_RELEASE == action) {
        enemyMoveDown = false;
    }

    GLFWFunctions::left_turn_flag = glfwGetKey(GLFWFunctions::pWindow, GLFW_KEY_LEFT) != 0;
    GLFWFunctions::right_turn_flag = glfwGetKey(GLFWFunctions::pWindow, GLFW_KEY_RIGHT) != 0;
    GLFWFunctions::scale_up_flag = glfwGetKey(GLFWFunctions::pWindow, GLFW_KEY_UP) != 0;
    GLFWFunctions::scale_down_flag = glfwGetKey(GLFWFunctions::pWindow, GLFW_KEY_DOWN) != 0;
    GLFWFunctions::move_up_flag = glfwGetKey(GLFWFunctions::pWindow, GLFW_KEY_W) != 0;
    GLFWFunctions::move_down_flag = glfwGetKey(GLFWFunctions::pWindow, GLFW_KEY_S) != 0;
    GLFWFunctions::move_left_flag = glfwGetKey(GLFWFunctions::pWindow, GLFW_KEY_A) != 0;
    GLFWFunctions::move_right_flag = glfwGetKey(GLFWFunctions::pWindow, GLFW_KEY_D) != 0;
    GLFWFunctions::move_jump_flag = glfwGetKey(GLFWFunctions::pWindow, GLFW_KEY_SPACE) != 0;
	GLFWFunctions::camera_zoom_in_flag = glfwGetKey(GLFWFunctions::pWindow, GLFW_KEY_Z) != 0;
	GLFWFunctions::camera_zoom_out_flag = glfwGetKey(GLFWFunctions::pWindow, GLFW_KEY_X) != 0;
	GLFWFunctions::camera_rotate_left_flag = glfwGetKey(GLFWFunctions::pWindow, GLFW_KEY_Q) != 0;
	GLFWFunctions::camera_rotate_right_flag = glfwGetKey(GLFWFunctions::pWindow, GLFW_KEY_E) != 0;


    if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

//Handle mouse button events
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

//Handle cursor position events
void GLFWFunctions::cursorPositionEvent(GLFWwindow* window, double xpos, double ypos) {
#ifdef _DEBUG
    std::cout << "Cursor position: " << xpos << ", " << ypos << std::endl;
#endif
}

//Handle scroll events
void GLFWFunctions::scrollEvent(GLFWwindow* window, double xoffset, double yoffset) {
#ifdef _DEBUG
    std::cout << "Scroll offset: " << xoffset << ", " << yoffset << std::endl;
#endif
}

//Caluclate the FPS and delta_time to be used
void GLFWFunctions::getFps() {

    static double prevTime = glfwGetTime();
    double currTime = glfwGetTime();
    GLFWFunctions::delta_time = static_cast<float>(currTime) - static_cast<float>(prevTime);
    prevTime = currTime;

    static double frameCount = 0;
    static double startTime = glfwGetTime();
    double elapsedTime = currTime - startTime;
    frameCount++;

    static double fpsInterval = 1.0;

    if (elapsedTime > fpsInterval) {
        GLFWFunctions::fps = frameCount / elapsedTime;
        if (GLFWFunctions::fps > 60) {
            fps = 60.0;
        }
        else {
            fps = GLFWFunctions::fps;
        }

        startTime = currTime;
        frameCount = 0.0;
    }

}

//terminates the window
void GLFWFunctions::glfwCleanup() {
    glfwTerminate();
}