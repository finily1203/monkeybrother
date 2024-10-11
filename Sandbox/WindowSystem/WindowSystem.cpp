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
		  						    (50%)
		 Joel Chu (c.weiyuan): Defined the functions for the WindowSystem class to
							   initialise, update and cleanup the window system.
							   (50%)
*//*___________________________________________________________________________-*/
#include <GraphicsSystem.h>
#include "Debug.h"
#include "WindowSystem.h"
#include <iostream>
#include "GlobalCoordinator.h"
#include "ECSCoordinator.h"


//GraphicsSystem graphicsSystem;
DebugSystem* DebugTool = new DebugSystem();
GraphicsSystem::GLObject gameObject, gameObject2, background, blackBox;
Shader* shader = nullptr;
Shader* shader2 = nullptr;
std::vector<GraphicsSystem::GLViewport> GraphicsSystem::vps;

void WindowSystem::logicUpdate() {

	if (GLFWFunctions::testMode == 1) {
		// Rotation logic
		if (GLFWFunctions::left_turn_flag) {
			gameObject.orientation.y = 180.0f * GLFWFunctions::delta_time * 2;
		}
		else if (GLFWFunctions::right_turn_flag) {
			gameObject.orientation.y = -180.0f * GLFWFunctions::delta_time * 2;
		}
		else {
			gameObject.orientation.y = 0.0f;
		}

		// Scaling logic
		if (GLFWFunctions::scale_up_flag) {
			if (gameObject.scaling.x < 600.0f && gameObject.scaling.y < 600.0f) {
				gameObject.scaling.x += 300.0f * GLFWFunctions::delta_time;
				gameObject.scaling.y += 300.0f * GLFWFunctions::delta_time;
			}
			graphicsSystem.SetCurrentAction(2);
		}
		else if (GLFWFunctions::scale_down_flag) {
			if (gameObject.scaling.x > 50.0f && gameObject.scaling.y > 50.0f) {
				gameObject.scaling.x -= 300.f * GLFWFunctions::delta_time;
				gameObject.scaling.y -= 300.0f * GLFWFunctions::delta_time;
			}
			graphicsSystem.SetCurrentAction(2);  // Action 2 for idle (third row)
		}

		// Movement logic
		bool isMoving = false;
		if (GLFWFunctions::move_up_flag) {
			gameObject.position.y += 100.0f * GLFWFunctions::delta_time;
			isMoving = true;
			graphicsSystem.SetCurrentAction(0);  // Action 2 for idle (third row)
		}
		if (GLFWFunctions::move_down_flag) {
			gameObject.position.y -= 100.0f * GLFWFunctions::delta_time;
			isMoving = true;
			graphicsSystem.SetCurrentAction(0);  // Action 2 for idle (third row)
		}
		if (GLFWFunctions::move_left_flag) {
			gameObject.position.x -= 100.0f * GLFWFunctions::delta_time;
			isMoving = true;
			graphicsSystem.SetCurrentAction(1);  // Action 0 for move left (second row)
		}
		if (GLFWFunctions::move_right_flag) {
			gameObject.position.x += 100.0f * GLFWFunctions::delta_time;
			isMoving = true;
			graphicsSystem.SetCurrentAction(3);  // Action 1 for move right (fourth row)
		}


		if (!isMoving) {
			graphicsSystem.SetCurrentAction(2);  // Action 2 for idle (third row)
		}
	}
}


void WindowSystem::initialise() {
	nlohmann::json windowConfigJSON;
	ECSCoordinator ecs;
	std::ifstream windowConfigFile(ecs.GetWindowConfigJSONPath());

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


	graphicsSystem.initialise();

	DebugTool->Initialise();

	gameObject.init(glm::vec2{ 0.0f, 0.0f }, glm::vec2{ 400.f, 400.0f }, glm::vec2{ -200.f, 0.0f });
	gameObject2.init(glm::vec2{ 0.0f, 0.0f }, glm::vec2{ 400.f, 400.0f }, glm::vec2{ 200.f, 0.0f });
	background.init(glm::vec2{ 0.0f, 0.0f }, glm::vec2{ 1600.f, 900.f }, glm::vec2{ 0.0f, 0.0f });
	blackBox.init(glm::vec2{ 0.0f, 0.0f }, glm::vec2{ 0.5f, 0.2f }, glm::vec2{ 0.0f, 0.0f });
	gameObject.is_animated = GL_TRUE;
	gameObject2.is_animated = GL_TRUE;
	background.is_animated = GL_FALSE;

	shader = graphicsSystem.GetShader();
	shader2 = graphicsSystem.GetShader2();
}

void WindowSystem::update() {

	glfwPollEvents();

	if (glfwWindowShouldClose(GLFWFunctions::pWindow)) {
		glfwSetWindowShouldClose(GLFWFunctions::pWindow, GLFW_TRUE);
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glClearColor(0.588f, 0.365f, 0.122f, 0.6f);

	// TODO:: Set the viewport incase the window was resized
	//GraphicsSystem::vps.push_back({ 0, 0, 1400, 700 });// TODO::change this to be calculated based on the window size

	//keyboardInputUpdateFlag();

	logicUpdate();

	DebugSystem::StartSystemTiming("Graphics");

	graphicsSystem.Update(GLFWFunctions::delta_time, false);
	graphicsSystem.Render(GLFWFunctions::delta_time);

	background.update(GLFWFunctions::delta_time);
	gameObject.update(GLFWFunctions::delta_time);
	gameObject2.update(GLFWFunctions::delta_time);
	blackBox.update(GLFWFunctions::delta_time);

	//graphicsSystem.Update(GLFWFunctions::delta_time, true);// TODO:: Check if object is animated and update accordingly

	if (GLFWFunctions::testMode == 1) {
		// Draw non-animated object (background)
		background.draw(shader, graphicsSystem.GetVAO(), graphicsSystem.GetTexture3());

		// Update animated objects before drawing them
		graphicsSystem.Update(GLFWFunctions::delta_time, true);

		// Draw game objects
		gameObject.draw(shader, graphicsSystem.GetVAO(), graphicsSystem.GetTexture());
		gameObject2.draw(shader, graphicsSystem.GetVAO(), graphicsSystem.GetTexture2());

		if (GLFWFunctions::debug_flag) {
			// Draw debug lines if debug mode is on
			graphicsSystem.drawDebugLines(gameObject);
			graphicsSystem.drawDebugLines(gameObject2);
		}

	}

	DebugSystem::EndSystemTiming("Graphics");

	DebugSystem::StartSystemTiming("Debug");
	DebugTool->Update();
	DebugSystem::EndSystemTiming("Debug");

	GLFWFunctions::getFps(1);

	//glfwSwapBuffers(GLFWFunctions::pWindow);

	GLenum error = glGetError(); if (error != GL_NO_ERROR) {
		std::cerr << "OpenGL Error: " << error << std::endl;
	}

}

void WindowSystem::cleanup() {
	//GLFWFunctions::glfwCleanup();
	graphicsSystem.cleanup();
	DebugTool->Cleanup();
}


