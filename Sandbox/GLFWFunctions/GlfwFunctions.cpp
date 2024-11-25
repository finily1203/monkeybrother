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
#include "GlobalCoordinator.h"
#include "LogicSystemECS.h"
#include <algorithm>
#include <iostream>
//#include <chrono>
//#include <thread>

//bool GLFWFunctions::isAKeyPressed = false;
//float GLFWFunctions::volume = 0.5f;
//float GLFWFunctions::zoomMouseCoordX = 0.0f;
//float GLFWFunctions::zoomMouseCoordY = 0.0f;
//float GLFWFunctions::objMoveMouseCoordX = 0.0f;
//float GLFWFunctions::objMoveMouseCoordY = 0.0f;
//bool GLFWFunctions::audioPaused = false;
//bool GLFWFunctions::audioNext = false;

//bool GLFWFunctions::audioStopped = false;
//bool GLFWFunctions::isGuiOpen = false;
//bool GLFWFunctions::zoomViewport = false;
//bool GLFWFunctions::isAtMaxZoom = false;
//bool GLFWFunctions::cloneObject = false;
//bool GLFWFunctions::goNextMode = false;
//bool GLFWFunctions::slideAudio = false;
//bool GLFWFunctions::bubblePopping = false;

//bool GLFWFunctions::enemyMoveLeft = false;
//bool GLFWFunctions::enemyMoveRight = false;
//bool GLFWFunctions::enemyMoveUp = false;
//bool GLFWFunctions::enemyMoveDown = false;

//GLboolean GLFWFunctions::left_turn_flag = false;
//GLboolean GLFWFunctions::right_turn_flag = false;
//GLboolean GLFWFunctions::scale_up_flag = false;
//GLboolean GLFWFunctions::scale_down_flag = false;
//GLboolean GLFWFunctions::move_up_flag = false;
//GLboolean GLFWFunctions::move_down_flag = false;
//GLboolean GLFWFunctions::move_left_flag = false;
//GLboolean GLFWFunctions::move_right_flag = false;

//GLboolean GLFWFunctions::move_jump_flag = false;

//GLboolean GLFWFunctions::camera_zoom_in_flag = false;
//GLboolean GLFWFunctions::camera_zoom_out_flag = false;
//GLboolean GLFWFunctions::camera_rotate_left_flag = false;
//GLboolean GLFWFunctions::camera_rotate_right_flag = false;

GLFWwindow* GLFWFunctions::pWindow = nullptr;
double GLFWFunctions::fps = 0.0;
float GLFWFunctions::delta_time = 0.0;


GLboolean GLFWFunctions::allow_camera_movement = false;
GLboolean GLFWFunctions::debug_flag = false;
GLboolean GLFWFunctions::isGuiOpen = false;
GLboolean GLFWFunctions::audioPaused = false;
GLboolean GLFWFunctions::audioStopped = false;
GLboolean GLFWFunctions::audioNext = false;
GLboolean GLFWFunctions::adjustVol = false;


int GLFWFunctions::audioNum = 0;
int GLFWFunctions::windowWidth = 0;
int GLFWFunctions::windowHeight = 0;
bool GLFWFunctions::bumpAudio = false;
bool GLFWFunctions::firstCollision = false;

std::unordered_map<Key, bool> GLFWFunctions::keyState;
std::unordered_map<MouseButton, bool> GLFWFunctions::mouseButtonState;


std::unordered_map<Key, bool> GLFWFunctions::keyState;
std::unordered_map<MouseButton, bool> GLFWFunctions::mouseButtonState;


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
    glfwSetDropCallback(GLFWFunctions::pWindow, GLFWFunctions::dropEvent);
}

//Handle keyboard events
void GLFWFunctions::keyboardEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {
	// unused parameters
	(void)scancode;
	(void)mods;

    Key mappedKey;
    switch (key) {
        // Alphabet keys
    case GLFW_KEY_A: mappedKey = Key::A; break;
    case GLFW_KEY_B: mappedKey = Key::B; break;
    case GLFW_KEY_C: mappedKey = Key::C; break;
    case GLFW_KEY_D: mappedKey = Key::D; break;
    case GLFW_KEY_E: mappedKey = Key::E; break;
    case GLFW_KEY_F: mappedKey = Key::F; break;
    case GLFW_KEY_G: mappedKey = Key::G; break;
    case GLFW_KEY_H: mappedKey = Key::H; break;
    case GLFW_KEY_I: mappedKey = Key::I; break;
    case GLFW_KEY_J: mappedKey = Key::J; break;
    case GLFW_KEY_K: mappedKey = Key::K; break;
    case GLFW_KEY_L: mappedKey = Key::L; break;
    case GLFW_KEY_M: mappedKey = Key::M; break;
    case GLFW_KEY_N: mappedKey = Key::N; break;
    case GLFW_KEY_O: mappedKey = Key::O; break;
    case GLFW_KEY_P: mappedKey = Key::P; break;
    case GLFW_KEY_Q: mappedKey = Key::Q; break;
    case GLFW_KEY_R: mappedKey = Key::R; break;
    case GLFW_KEY_S: mappedKey = Key::S; break;
    case GLFW_KEY_T: mappedKey = Key::T; break;
    case GLFW_KEY_U: mappedKey = Key::U; break;
    case GLFW_KEY_V: mappedKey = Key::V; break;
    case GLFW_KEY_W: mappedKey = Key::W; break;
    case GLFW_KEY_X: mappedKey = Key::X; break;
    case GLFW_KEY_Y: mappedKey = Key::Y; break;
    case GLFW_KEY_Z: mappedKey = Key::Z; break;

        // Function keys
    case GLFW_KEY_F1: mappedKey = Key::F1; break;
    case GLFW_KEY_F2: mappedKey = Key::F2; break;
    case GLFW_KEY_F3: mappedKey = Key::F3; break;
    case GLFW_KEY_F4: mappedKey = Key::F4; break;
    case GLFW_KEY_F5: mappedKey = Key::F5; break;
    case GLFW_KEY_F6: mappedKey = Key::F6; break;
    case GLFW_KEY_F7: mappedKey = Key::F7; break;
    case GLFW_KEY_F8: mappedKey = Key::F8; break;
    case GLFW_KEY_F9: mappedKey = Key::F9; break;
    case GLFW_KEY_F10: mappedKey = Key::F10; break;
    case GLFW_KEY_F11: mappedKey = Key::F11; break;
    case GLFW_KEY_F12: mappedKey = Key::F12; break;

        // Control and special keys
    case GLFW_KEY_SPACE:        mappedKey = Key::SPACE; break;
    case GLFW_KEY_ESCAPE:       mappedKey = Key::ESCAPE; break;
    case GLFW_KEY_LEFT:         mappedKey = Key::LEFT; break;
    case GLFW_KEY_RIGHT:        mappedKey = Key::RIGHT; break;
    case GLFW_KEY_UP:           mappedKey = Key::UP; break;
    case GLFW_KEY_DOWN:         mappedKey = Key::DOWN; break;
    case GLFW_KEY_LEFT_SHIFT:   mappedKey = Key::LSHIFT; break;
    case GLFW_KEY_LEFT_CONTROL: mappedKey = Key::LCTRL; break;
    case GLFW_KEY_LEFT_ALT:     mappedKey = Key::LALT; break;
    case GLFW_KEY_COMMA:        mappedKey = Key::COMMA; break;
    case GLFW_KEY_PERIOD:       mappedKey = Key::PERIOD; break;

        // Number keys
    case GLFW_KEY_0:            mappedKey = Key::NUM_0; break;
    case GLFW_KEY_1:            mappedKey = Key::NUM_1; break;
    case GLFW_KEY_2:            mappedKey = Key::NUM_2; break;
    case GLFW_KEY_3:            mappedKey = Key::NUM_3; break;
    case GLFW_KEY_4:            mappedKey = Key::NUM_4; break;
    case GLFW_KEY_5:            mappedKey = Key::NUM_5; break;
    case GLFW_KEY_6:            mappedKey = Key::NUM_6; break;
    case GLFW_KEY_7:            mappedKey = Key::NUM_7; break;
    case GLFW_KEY_8:            mappedKey = Key::NUM_8; break;
    case GLFW_KEY_9:            mappedKey = Key::NUM_9; break;


        // Default case
    default: return; // Use Key::UNKNOWN for unmapped keys if defined.
    }

    if (action == GLFW_PRESS) {
        keyState[mappedKey] = true;
    }
    else if (action == GLFW_RELEASE) {
        keyState[mappedKey] = false;
    }
    else if (action == GLFW_REPEAT) {
        keyState[mappedKey] = true;
    }
    if (!GameViewWindow::getPaused()) {
        if (keyState[Key::NUM_2])
            allow_camera_movement = ~allow_camera_movement;

        if (keyState[Key::NUM_1]) {
            debug_flag = ~debug_flag;
            isGuiOpen = ~isGuiOpen;
        }
    }
    

    if (keyState[Key::P])
        audioPaused = ~audioPaused;

    if (keyState[Key::S])
        audioStopped = ~audioStopped;

    if (keyState[Key::N]) {
        audioNext = ~audioNext;
        GLFWFunctions::audioNum = (GLFWFunctions::audioNum + 1) % 2;
    }

    if (keyState[Key::ESCAPE]) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
	(void)window;
}

//Handle mouse button events
void GLFWFunctions::mouseButtonEvent(GLFWwindow* window, int button, int action, int mods) {
    (void)mods;
    MouseButton mappedButton;
    switch (button) {
    case GLFW_MOUSE_BUTTON_LEFT: mappedButton = MouseButton::left; break;
    case GLFW_MOUSE_BUTTON_RIGHT: mappedButton = MouseButton::right; break;
    case GLFW_MOUSE_BUTTON_MIDDLE: mappedButton = MouseButton::middle; break;
    default: return;
    }

    if (action == GLFW_PRESS) {
        mouseButtonState[mappedButton] = true;

        if (mappedButton == MouseButton::left)
        {
            MouseBehaviour click;
            double mouseX{}, mouseY{};
            int windowWidth{}, windowHeight{};
            glfwGetCursorPos(pWindow, &mouseX, &mouseY);
            glfwGetWindowSize(pWindow, &windowWidth, &windowHeight);

            float cursorXCentered = static_cast<float>(mouseX) - (windowWidth / 2.f);
            float cursorYCentered = (windowHeight / 2.f) - static_cast<float>(mouseY);
            click.onMouseClick(window, static_cast<double>(cursorXCentered), static_cast<double>(cursorYCentered));
        }
    }
    else if (action == GLFW_RELEASE) {
        mouseButtonState[mappedButton] = false;
    }

	(void)window;
}

//Handle cursor position events
void GLFWFunctions::cursorPositionEvent(GLFWwindow* window, double xpos, double ypos) {
    //On relase it doesn't use since we use cursorPositionEvent for debugging
    (void)window; (void)xpos; (void)ypos;
#ifdef _DEBUG
    std::cout << "Cursor position: " << xpos << ", " << ypos << std::endl;
#endif
}

//Handle scroll events
void GLFWFunctions::scrollEvent(GLFWwindow* window, double xoffset, double yoffset) {
	//On relase it doesn't use since we use ScrollEvent for debugging
    (void)window; (void)xoffset; (void)yoffset;
    
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

void GLFWFunctions::dropEvent(GLFWwindow* window, int count, const char** paths) {
    (void)window;
    for (int i = 0; i < count; i++) {
        std::string filePath = paths[i];
        assetsManager.handleDropFile(filePath);
    }
}

//terminates the window
void GLFWFunctions::glfwCleanup() {
    glfwTerminate();
}

// Input state functions
bool GLFWFunctions::isKeyPressed(Key key) {
	return glfwGetKey(GLFWFunctions::pWindow, static_cast<int>(key)) == GLFW_PRESS;
}

bool GLFWFunctions::isKeyReleased(Key key) {
	return glfwGetKey(GLFWFunctions::pWindow, static_cast<int>(key)) == GLFW_RELEASE;
}

bool GLFWFunctions::isKeyHeld(Key key) {
	return glfwGetKey(GLFWFunctions::pWindow, static_cast<int>(key)) == GLFW_REPEAT;
}

bool GLFWFunctions::isMouseButtonPressed(MouseButton button) {
	return glfwGetMouseButton(GLFWFunctions::pWindow, static_cast<int>(button)) == GLFW_PRESS;
}
