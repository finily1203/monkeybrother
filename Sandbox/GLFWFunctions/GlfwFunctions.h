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
#include <unordered_map>
#include "TransformComponent.h"
#include "ButtonComponent.h"

enum class Key {
	A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
	F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
	LEFT, RIGHT, UP, DOWN,
	LSHIFT, LCTRL, LALT, SPACE, ESCAPE, COMMA, PERIOD,
	NUM_0, NUM_1, NUM_2, NUM_3, NUM_4, NUM_5, NUM_6, NUM_7, NUM_8, NUM_9
};

enum class MouseButton {
	left, right, middle
};

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
	//Handle Drag and Drop events
	static void dropEvent(GLFWwindow* window, int count, const char** paths);
	//Handle window to check for events
	static void callEvents();

	static void handleMouseClick(GLFWwindow* window, double mouseX, double mouseY);
	static bool mouseIsOverButton(double mouseX, double mouseY, TransformComponent& transform);

	//terminates the window
	static void glfwCleanup();

	//Caluclate the FPS and delta_time to be used
	static void getFps();

	// Input state functions
	static bool isKeyPressed(Key key);
	static bool isKeyReleased(Key key);
	static bool isKeyHeld(Key key);

	static bool isMouseButtonPressed(MouseButton button);

	// key state



	//static bool isAKeyPressed;

	//static float volume;
	//static float zoomMouseCoordX;
	//static float zoomMouseCoordY;
	//static float objMoveMouseCoordX;
	//static float objMoveMouseCoordY;
	//static bool isAtMaxZoom;
	//static bool audioPaused;
	//static bool audioNext;

	//static bool audioStopped;
	//static bool cloneObject;
	//static bool goNextMode;

	//static bool slideAudio;
	//static bool bubblePopping;

	//static bool enemyMoveLeft;
	//static bool enemyMoveRight;
	//static bool enemyMoveUp;
	//static bool enemyMoveDown;

	//static GLboolean left_turn_flag;
	//static GLboolean right_turn_flag;
	//static GLboolean scale_up_flag;
	//static GLboolean scale_down_flag;
	//static GLboolean move_up_flag;
	//static GLboolean move_down_flag;
	//static GLboolean move_left_flag;
	//static GLboolean move_right_flag;

	//static GLboolean move_jump_flag;

	//static GLboolean camera_zoom_in_flag;
	//static GLboolean camera_zoom_out_flag;
	//static GLboolean camera_rotate_left_flag;
	//static GLboolean camera_rotate_right_flag;

	//static bool isGuiOpen;
	//static bool zoomViewport;
	//static int testMode;

	static GLFWwindow* pWindow;
	static double fps;
	static float delta_time;


	static GLboolean debug_flag;
	static GLboolean isGuiOpen;
	static GLboolean allow_camera_movement;
	static GLboolean audioPaused;
	static GLboolean audioStopped;
	static GLboolean adjustVol;
	static GLboolean audioNext;


	static int audioNum;
	static int windowWidth;
	static int windowHeight;
	static bool bumpAudio;
	static bool firstCollision;


	static std::unordered_map<Key, bool> keyState;
	static std::unordered_map<MouseButton, bool> mouseButtonState;
};

#endif
//END HEADER GUARDS//