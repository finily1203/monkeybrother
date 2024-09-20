#include "WindowSystem.h"
#include "Debug.h"
#include <iostream>

void WindowSystem::initialise() {
	if (!GLFWFunctions::init(1600, 900, "Testing Application 123")) {
		std::cout << "Failed to initialise GLFW" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void WindowSystem::update() {
	glfwPollEvents();
	GLFWFunctions::getFps(1);

	if (glfwWindowShouldClose(GLFWFunctions::pWindow)) {
		glfwSetWindowShouldClose(GLFWFunctions::pWindow, GLFW_TRUE);
	}
}

void WindowSystem::cleanup() {
	GLFWFunctions::glfwCleanup();
}
