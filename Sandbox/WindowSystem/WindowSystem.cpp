#include <GraphicsSystem.h>
#include "Debug.h"
#include "WindowSystem.h"
#include <iostream>


GraphicsSystem graphicsSystem;
DebugSystem* DebugTool = new DebugSystem();

void WindowSystem::initialise() {
	nlohmann::json windowConfigJSON;
	std::ifstream windowConfigFile("./Serialization/windowConfig.json");

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

	if (!GLFWFunctions::init(windowWidth, windowHeight, windowTitle.c_str())) {
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
	
}

void WindowSystem::cleanup() {
	//GLFWFunctions::glfwCleanup();
	graphicsSystem.Cleanup();
	DebugTool->Cleanup();
}
