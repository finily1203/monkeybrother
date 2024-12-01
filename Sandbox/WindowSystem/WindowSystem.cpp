/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Liu YaoTing (yaoting.liu), Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   WindowSystem.cpp
@brief:  This source file defines the WindowSystem class. The WindowSystem class is
	 used to handle the window application for the game engine. It will initialise,
	 update and cleanup the window system.
	 Liu YaoTing (yaoting.liu): Defined the logic for objects for graphics system
				  for the WindowSystem class as well as the init, update,
				  and rendering of those objects
				  (27.5%)
	 Joel Chu (c.weiyuan): Defined the functions for the WindowSystem class to
				 initialise, update and cleanup the window system.
				 (27.5%)
	 Lee Jing Wen (jingwen.lee): Defined the function to handle window focus
				   (interruption handling)
				   (45%)
*//*___________________________________________________________________________-*/
#include <GraphicsSystem.h>
#include "Debug.h"
#include "WindowSystem.h"
#include <iostream>
#include "GlobalCoordinator.h"
#include "ECSCoordinator.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

bool WindowSystem::altTab = false;
bool WindowSystem::ctrlAltDel = false;

void WindowSystem::initialise() {
	nlohmann::json windowConfigJSON;
	std::ifstream windowConfigFile(FilePathManager::GetWindowConfigJSONPath());

	if (!windowConfigFile.is_open())
	{
		std::cout << "Unable to open JSON file" << std::endl;
		return;
	}

	windowConfigFile >> windowConfigJSON;
	windowConfigFile.close();

	int windowWidth = windowConfigJSON["width"].get<int>();
	int windowHeight = windowConfigJSON["height"].get<int>();
	std::string windowTitle = windowConfigJSON["title"].get<std::string>();
	bool startFullscreen = windowConfigJSON["fullscreen"].get<bool>();

	if (!GLFWFunctions::init(windowWidth, windowHeight, windowTitle, startFullscreen)) {
		std::cout << "Failed to initialise GLFW" << std::endl;
		exit(EXIT_FAILURE);
	}

	GLenum glewInitResult = glewInit(); if (glewInitResult != GLEW_OK) {
		std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(glewInitResult) << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	std::cout << "GLEW initialized successfully" << std::endl;

	int width, height; glfwGetFramebufferSize(GLFWFunctions::pWindow, &width, &height);
}

//Handle interruption, when window is not focused
void WindowSystem::handleWindowFocus()
{
	static bool securityScreenActive = false;
	isFocused = glfwGetWindowAttrib(GLFWFunctions::pWindow, GLFW_FOCUSED);
	HWND appWindow = glfwGetWin32Window(GLFWFunctions::pWindow);

	// Check for Ctrl+Alt+Del press
	bool keysArePressed = (GetAsyncKeyState(VK_CONTROL) & 0x8000) &&
		(GetAsyncKeyState(VK_MENU) & 0x8000) &&
		(GetAsyncKeyState(VK_DELETE) & 0x8000);

	// When Ctrl+Alt+Del is pressed
	if (keysArePressed && !keysWerePressed)
	{
		securityScreenActive = true;
		GLFWFunctions::audioPaused = true;
		glfwIconifyWindow(GLFWFunctions::pWindow);
		ctrlAltDel = true;
		altTab = false;
	}

	// When returning from security screen
	if (securityScreenActive && !keysArePressed)
	{
		//Get the currently active window
		HWND activeWindow = GetForegroundWindow();

		//If we're back to desktop/game window, restore
		if (activeWindow != NULL && activeWindow != appWindow)
		{
			securityScreenActive = false;
		}
	}

	// Handle normal focus changes
	if (!securityScreenActive)
	{
		if (isFocused)
		{
			GLFWFunctions::audioPaused = false;
			glfwRestoreWindow(GLFWFunctions::pWindow); // Comment this if want to restore window when returning from security screen
			altTab = false;
			ctrlAltDel = false;
		}
		else
		{
			GLFWFunctions::audioPaused = true;
			glfwIconifyWindow(GLFWFunctions::pWindow); // Comment this if want to restore window when returning from security screen
			altTab = true;
		}
	}

	// Update previous key state
	keysWerePressed = keysArePressed;
}

void WindowSystem::update() {

	glfwPollEvents();

	handleWindowFocus();

	if (glfwWindowShouldClose(GLFWFunctions::pWindow)) {
		glfwSetWindowShouldClose(GLFWFunctions::pWindow, GLFW_TRUE);
	}

	glfwGetFramebufferSize(GLFWFunctions::pWindow, &GLFWFunctions::windowWidth, &GLFWFunctions::windowHeight);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glClearColor(0.588f, 0.365f, 0.122f, 0.6f);

	GLFWFunctions::getFps();


	GLenum error = glGetError(); if (error != GL_NO_ERROR) {
		std::cerr << "OpenGL Error: " << error << std::endl;
	}

}

void WindowSystem::cleanup() {
	//Console::Cleanup();
	graphicsSystem.cleanup();
	GLFWFunctions::glfwCleanup();
}

//For perfomance viewer
SystemType WindowSystem::getSystem() {
	return WindowSystemType;
}

