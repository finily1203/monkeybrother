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
#include <Windows.h>

GLFWwindow* GLFWFunctions::pWindow = nullptr;
double GLFWFunctions::fps = 0.0;
float GLFWFunctions::delta_time = 0.0;


bool GLFWFunctions::allow_camera_movement = false;
bool GLFWFunctions::debug_flag = false;
GLboolean GLFWFunctions::isGuiOpen = false;
GLboolean GLFWFunctions::audioPaused = false;
GLboolean GLFWFunctions::audioStopped = false;
GLboolean GLFWFunctions::audioNext = false;
GLboolean GLFWFunctions::adjustVol = false;
GLboolean GLFWFunctions::instantWin = false;
GLboolean GLFWFunctions::instantLose = false;
GLboolean GLFWFunctions::skipToNextLevel = false;
GLboolean GLFWFunctions::skipToEnd = false;
GLboolean GLFWFunctions::godMode = false;
bool GLFWFunctions::fullscreen = false;
GLboolean GLFWFunctions::isPumpOn = true;
GLboolean GLFWFunctions::playPumpSong = true;
GLboolean GLFWFunctions::isRotating = false;
GLboolean GLFWFunctions::exitCollision = false;
bool GLFWFunctions::useMouseRotation = false; // Default to mouse rotation


int GLFWFunctions::audioNum = 0;
int GLFWFunctions::windowWidth = 0;
int GLFWFunctions::windowHeight = 0;
int GLFWFunctions::defultWindowWidth = 0;
int GLFWFunctions::defultWindowHeight = 0;
int GLFWFunctions::collectableCount = 0;
int GLFWFunctions::pauseMenuCount = 0;
int GLFWFunctions::optionsMenuCount = 0;
bool GLFWFunctions::bumpAudio = false;
bool GLFWFunctions::collectAudio = false;
bool GLFWFunctions::firstCollision = false;
bool GLFWFunctions::gameOver = false;
bool GLFWFunctions::isHovering = false;
bool GLFWFunctions::gamePaused = false;
MouseBehaviour mouseBehaviour;
double GLFWFunctions::mouseXDelta = 0.0;

std::unordered_map<Key, bool>* GLFWFunctions::keyState = nullptr;
std::unordered_map<MouseButton, bool>* GLFWFunctions::mouseButtonState;


// Initialize the window
bool GLFWFunctions::init(int width, int height, std::string title, bool isfullscreen) {

    /* Initialize the library */
    if (!glfwInit())
        return false;

    if (!keyState)
        keyState = new std::unordered_map<Key, bool>();

    fullscreen = isfullscreen;

    // Determine if fullscreen or windowed mode
    GLFWmonitor* monitor = nullptr;
    if (isfullscreen) {
        monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        windowWidth = mode->width;
        windowHeight = mode->height;
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    }
    else {
        windowWidth = width;
        windowHeight = height;
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // Make window non-resizable
    }
    // Set the window width and height
    defultWindowHeight = height;
    defultWindowWidth = width;

    /* Create a windowed mode window and its OpenGL context */
    GLFWFunctions::pWindow = glfwCreateWindow(windowWidth, windowHeight, title.c_str(), NULL, NULL);
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
    /*if (useMouseRotation) {
		glfwSetInputMode(GLFWFunctions::pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    else
        glfwSetInputMode(GLFWFunctions::pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);*/
    glfwSetInputMode(GLFWFunctions::pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    //glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
    (void)window;

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
	case GLFW_KEY_ENTER:        mappedKey = Key::ENTER; break;

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
        (*keyState)[mappedKey] = true;
    }
    else if (action == GLFW_RELEASE) {
        (*keyState)[mappedKey] = false;
    }
    else if (action == GLFW_REPEAT) {
        (*keyState)[mappedKey] = true;
    }

    if ((*keyState)[Key::NUM_2])
        allow_camera_movement = !allow_camera_movement;

    if (!GameViewWindow::getPaused()) {

        if ((*keyState)[Key::NUM_1]) {
            debug_flag = !debug_flag;
            isGuiOpen = ~isGuiOpen;
        }
    }

    if ((*keyState)[Key::A] || (*keyState)[Key::D]) {
        isRotating = true;
    }
    else {
        isRotating = false;
    }

    if ((*keyState)[Key::P] && GameViewWindow::getSceneNum() > -1 && !GameViewWindow::getPaused()) {
        glfwSetInputMode(GLFWFunctions::pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        audioPaused = ~audioPaused;
        GLFWFunctions::gamePaused = true;

        if (GLFWFunctions::pauseMenuCount < 1 && GLFWFunctions::optionsMenuCount != 1)
        {
            GLFWFunctions::pauseMenuCount++;
            ecsCoordinator.LoadPauseMenuFromJSON(ecsCoordinator, FilePathManager::GetPauseMenuJSONPath());
        }

        else if (GLFWFunctions::optionsMenuCount != 1)
        {
            for (auto currEntity : ecsCoordinator.getAllLiveEntities())
            {
                if (ecsCoordinator.getEntityID(currEntity) == "pauseMenuBg" ||
                    ecsCoordinator.getEntityID(currEntity) == "closePauseMenu" ||
                    ecsCoordinator.getEntityID(currEntity) == "resumeButton" ||
                    ecsCoordinator.getEntityID(currEntity) == "pauseOptionsButton" ||
                    ecsCoordinator.getEntityID(currEntity) == "pauseTutorialButton" ||
                    ecsCoordinator.getEntityID(currEntity) == "pauseQuitButton")
                {
                    ecsCoordinator.destroyEntity(currEntity);
                }
            }

            GLFWFunctions::gamePaused = false;
            GLFWFunctions::pauseMenuCount--;
        }
    }

    if ((*keyState)[Key::S])
        audioStopped = ~audioStopped;

    if ((*keyState)[Key::N]) {
        audioNext = ~audioNext;
        GLFWFunctions::audioNum = (GLFWFunctions::audioNum + 1) % 2;
    }

    if ((*keyState)[Key::ESCAPE]) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    if (action == GLFW_PRESS) {

        // Cheat codes
        if ((*keyState)[Key::G] && (*keyState)[Key::O] && (*keyState)[Key::D]) {
            godMode = ~godMode;
            std::cout << "God Mode: " << (godMode ? "ON" : "OFF") << std::endl;
        }

        if ((*keyState)[Key::N] && (*keyState)[Key::E] && (*keyState)[Key::X] && (*keyState)[Key::T]) {
            skipToNextLevel = true;
            std::cout << "Skip to Next Level" << std::endl;
        }

        if ((*keyState)[Key::E] && (*keyState)[Key::N] && (*keyState)[Key::D]) {
            skipToEnd = true;
            std::cout << "Skip to End" << std::endl;
        }

        if ((*keyState)[Key::W] && (*keyState)[Key::I] && (*keyState)[Key::N]) {
            instantWin = true;
            std::cout << "Instant Win" << std::endl;
        }

        if ((*keyState)[Key::L] && (*keyState)[Key::O] && (*keyState)[Key::S] && (*keyState)[Key::E]) {
            instantLose = true;
            std::cout << "Instant Lose" << std::endl;
        }

        if ((*keyState)[Key::F] && action == GLFW_PRESS && !GameViewWindow::getPaused()) {
            fullscreen = !fullscreen;
            GLFWmonitor* monitor = fullscreen ? glfwGetPrimaryMonitor() : nullptr;

            if (fullscreen) {
                const GLFWvidmode* mode = glfwGetVideoMode(monitor);
                glfwSetWindowMonitor(
                    pWindow,
                    monitor,
                    0, 0,
                    mode->width,
                    mode->height,
                    mode->refreshRate
                );
            }
            else {
                // Switch to windowed mode
                int windowedWidth = defultWindowWidth;  // Desired windowed mode width
                int windowedHeight = defultWindowHeight; // Desired windowed mode height
                int x = 150;             // Desired X position for the window
                int y = 150;             // Desired Y position for the window

                glfwSetWindowMonitor(
                    pWindow,
                    nullptr,
                    x, y,
                    windowedWidth,
                    windowedHeight,
                    0
                );

                // Restore window decorations
                glfwSetWindowAttrib(pWindow, GLFW_DECORATED, GLFW_TRUE);
                glfwSetWindowAttrib(pWindow, GLFW_RESIZABLE, GLFW_FALSE);
            }

            std::cout << "Fullscreen: " << (fullscreen ? "ON" : "OFF") << std::endl;
        }

    }
    else if (action == GLFW_RELEASE) {
        (*keyState)[static_cast<Key>(key)] = false;
    }
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
        if (!mouseButtonState) {
            mouseButtonState = new std::unordered_map<MouseButton, bool>();
        }
        mouseButtonState->operator[](mappedButton) = true;

        if (mappedButton == MouseButton::left)
        {
            double mouseX{}, mouseY{};
            int currWindowWidth{}, currWindowHeight{};
            glfwGetCursorPos(pWindow, &mouseX, &mouseY);
            glfwGetWindowSize(pWindow, &currWindowWidth, &currWindowHeight);

            float cursorXCentered = static_cast<float>(mouseX) - (windowWidth / 2.f);
            float cursorYCentered = (windowHeight / 2.f) - static_cast<float>(mouseY);

            if (!debug_flag)
            {
                mouseBehaviour.onMouseClick(window, static_cast<double>(cursorXCentered), static_cast<double>(cursorYCentered));
            }
        }
    }
    else if (action == GLFW_RELEASE) {
        mouseButtonState->operator[](mappedButton) = false;

        if (mappedButton == MouseButton::left)
        {
            mouseBehaviour.setIsDragging(false);
        }
    }

    (void)window;
}

//Handle cursor position events
void GLFWFunctions::cursorPositionEvent(GLFWwindow* window, double xpos, double ypos) {
    (void)window;
    (void)ypos;
    static double lastX = xpos;
    mouseXDelta = xpos - lastX;
    lastX = xpos;
    //On relase it doesn't use since we use cursorPositionEvent for debugging
    if (mouseBehaviour.getIsDragging())
    {
        mouseBehaviour.onMouseDrag(window, xpos, ypos);
    }
#ifdef _DEBUG
    std::cout << "Cursor position: " << xpos << ", " << ypos << std::endl;
    if (mouseBehaviour.getIsDragging())
    {
        mouseBehaviour.onMouseDrag(window, xpos, ypos);
    }
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
        std::string fileExtension = filePath.substr(filePath.find_last_of('.'));
        if (fileExtension == ".ogg" || fileExtension == ".txt") { //might need to add more file types
            //seperate window to inform not allowed to drop type of file
            MessageBoxA(nullptr, ("Type of file \"" + fileExtension + "\" is not allowed to be used.").c_str(), "Incorrect File Type", MB_OK | MB_ICONERROR);
        }
        else {
            assetsManager.handleDropFile(filePath);
        }
    }
}

//terminates the window
void GLFWFunctions::glfwCleanup() {

    if (keyState) {
        delete keyState;
        keyState = nullptr;
    }
    if (mouseButtonState) {
        delete mouseButtonState;
        mouseButtonState = nullptr;
    }

    mouseBehaviour.~MouseBehaviour();

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

void GLFWFunctions::updateCursorState() {
    if (useMouseRotation) {
        // Hide cursor and capture it for mouse rotation
        glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    else {
        // Show cursor and allow normal movement
        glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}