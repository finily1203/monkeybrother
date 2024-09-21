#include <GraphicsSystem.h>
#include "Debug.h"
#include "WindowSystem.h"
#include <iostream>


GraphicsSystem graphicsSystem;

void WindowSystem::initialise() {
	if (!GLFWFunctions::init(1600, 900, "Testing Application 123")) {
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
	glViewport(0, 0, width, height);
	// Set the clear colorglClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	graphicsSystem.Initialize();
}

void WindowSystem::update() {
	glfwPollEvents();
	GLFWFunctions::getFps(1);

	if (glfwWindowShouldClose(GLFWFunctions::pWindow)) {
		glfwSetWindowShouldClose(GLFWFunctions::pWindow, GLFW_TRUE);
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	graphicsSystem.Render();
	glfwSwapBuffers(GLFWFunctions::pWindow);
	glfwPollEvents();

	GLenum error = glGetError(); if (error != GL_NO_ERROR) {
		std::cerr << "OpenGL Error: " << error << std::endl;
	}
}

void WindowSystem::cleanup() {
	GLFWFunctions::glfwCleanup();
	graphicsSystem.Cleanup();
}
