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
bool GLFWFunctions::audioPaused = false;
bool GLFWFunctions::audioNext = false;
int GLFWFunctions::audioNum = 0;
bool GLFWFunctions::audioStopped = false;
bool GLFWFunctions::isGuiOpen = false;
bool GLFWFunctions::zoomViewport = false;
bool GLFWFunctions::isAtMaxZoom = false;
bool GLFWFunctions::cloneObject = false;
bool GLFWFunctions::goNextMode = false;

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

int GLFWFunctions::testMode = 0;

// Initialize the window
bool GLFWFunctions::init(int width, int height, std::string title) {

    /* Initialize the library */
    if (!glfwInit())
        return false;

    /* Create a windowed mode window and its OpenGL context */
    GLFWFunctions::pWindow = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    if (!GLFWFunctions::pWindow)
    {
        throw CrashLog::Exception("Failed to create window", __FILE__, __LINE__);
        //std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        return false;
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

    if (GLFW_KEY_L == key && GLFW_PRESS == action && GLFWFunctions::audioStopped == false) {
        GLFWFunctions::audioStopped = true;
    }
    else if (GLFW_KEY_L == key && GLFW_PRESS == action && GLFWFunctions::audioStopped == true) {
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

    GLFWFunctions::left_turn_flag = glfwGetKey(GLFWFunctions::pWindow, GLFW_KEY_LEFT) != 0;
    GLFWFunctions::right_turn_flag = glfwGetKey(GLFWFunctions::pWindow, GLFW_KEY_RIGHT) != 0;
    GLFWFunctions::scale_up_flag = glfwGetKey(GLFWFunctions::pWindow, GLFW_KEY_UP) != 0;
    GLFWFunctions::scale_down_flag = glfwGetKey(GLFWFunctions::pWindow, GLFW_KEY_DOWN) != 0;
    GLFWFunctions::move_up_flag = glfwGetKey(GLFWFunctions::pWindow, GLFW_KEY_W) != 0;
    GLFWFunctions::move_down_flag = glfwGetKey(GLFWFunctions::pWindow, GLFW_KEY_S) != 0;
    GLFWFunctions::move_left_flag = glfwGetKey(GLFWFunctions::pWindow, GLFW_KEY_A) != 0;
    GLFWFunctions::move_right_flag = glfwGetKey(GLFWFunctions::pWindow, GLFW_KEY_D) != 0;
    GLFWFunctions::move_jump_flag = glfwGetKey(GLFWFunctions::pWindow, GLFW_KEY_SPACE) != 0;


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

    //double mouseX, mouseY;
    //glfwGetCursorPos(window, &mouseX, &mouseY);

    //// Check if mouse is inside viewport using the static method
    //if (GameViewWindow::IsPointInViewport(mouseX, mouseY) &&
    //    GLFW_MOUSE_BUTTON_LEFT == button &&
    //    GLFW_PRESS == action &&
    //    GLFWFunctions::isGuiOpen) {

    //    GLFWFunctions::zoomMouseCoordX = mouseX;
    //    GLFWFunctions::zoomMouseCoordY = mouseY;
    //    GLFWFunctions::zoomViewport = !GLFWFunctions::zoomViewport;
    //}
}

//Handle cursor position events
void GLFWFunctions::cursorPositionEvent(GLFWwindow* window, double xpos, double ypos) {
#ifdef _DEBUG
    std::cout << "Cursor position: " << xpos << ", " << ypos << std::endl;
    /*zoomMouseCoordX = xpos;
    zoomMouseCoordY = ypos;*/
#endif
}

//Handle scroll events
void GLFWFunctions::scrollEvent(GLFWwindow* window, double xoffset, double yoffset) {
#ifdef _DEBUG
    std::cout << "Scroll offset: " << xoffset << ", " << yoffset << std::endl;
#endif

    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    if (GameViewWindow::IsPointInViewport(mouseX, mouseY) &&
        GLFWFunctions::isGuiOpen) {
        float zoomDelta = yoffset * 0.1f;
        float newZoomLevel = GameViewWindow::zoomLevel + zoomDelta;

        //// Check for max zoom condition
        //if (GameViewWindow::zoomLevel >= GameViewWindow::MAX_ZOOM && zoomDelta > 0) {
        //    isAtMaxZoom = true;
        //    return;
        //}
        //isAtMaxZoom = false; // Reset the flag when not at max zoom

        // Update coordinates and zoom level
        GLFWFunctions::zoomMouseCoordX = mouseX;
        GLFWFunctions::zoomMouseCoordY = mouseY;

        GameViewWindow::zoomLevel = std::min(GameViewWindow::MAX_ZOOM,
            std::max(GameViewWindow::MIN_ZOOM,
                newZoomLevel));

        GLFWFunctions::zoomViewport = (GameViewWindow::zoomLevel != 1.0f);
    }
}

//Caluclate the FPS and delta_time to be used
void GLFWFunctions::getFps(double fpsCalculateInt) {

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