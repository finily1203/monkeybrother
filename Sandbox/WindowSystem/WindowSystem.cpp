#include <GraphicsSystem.h>
#include "Debug.h"
#include "WindowSystem.h"
#include <iostream>


GraphicsSystem graphicsSystem;
DebugSystem* DebugTool = new DebugSystem();

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

	DebugTool->Initialise();
}

void WindowSystem::update() {
	DebugSystem::StartSystemTiming("Windows");
	glfwPollEvents();

	if (glfwWindowShouldClose(GLFWFunctions::pWindow)) {
		glfwSetWindowShouldClose(GLFWFunctions::pWindow, GLFW_TRUE);
	}
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DebugSystem::StartSystemTiming("Graphics"); //Get start of graphics gameloop
	graphicsSystem.Render();
	DebugSystem::EndSystemTiming("Graphics"); //Get end of graphics gameloop

	DebugSystem::StartSystemTiming("Debug"); //Get start of debug gameloop
	DebugTool->Update();
	DebugSystem::EndSystemTiming("Debug"); //Get end of debug gameloop

	glfwSwapBuffers(GLFWFunctions::pWindow);
	
	glfwPollEvents();
	GLFWFunctions::getFps(1);
	GLenum error = glGetError(); if (error != GL_NO_ERROR) {
		std::cerr << "OpenGL Error: " << error << std::endl;
	}
	DebugSystem::EndSystemTiming("Windows");
}

void WindowSystem::cleanup() {
	//GLFWFunctions::glfwCleanup();
	graphicsSystem.Cleanup();
	DebugTool->Cleanup();
}
