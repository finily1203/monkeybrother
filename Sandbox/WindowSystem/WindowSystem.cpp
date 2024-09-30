#include <GraphicsSystem.h>
#include "Debug.h"
#include "WindowSystem.h"
#include <iostream>


GraphicsSystem graphicsSystem;
DebugSystem* DebugTool = new DebugSystem();
GraphicsSystem::GLObject gameObject, gameObject2, background, blackBox;
Shader* shader = nullptr;
GLboolean left_turn_flag = false;
GLboolean right_turn_flag = false;
GLboolean scale_up_flag = false;
GLboolean scale_down_flag = false;
GLboolean move_up_flag = false;
GLboolean move_down_flag = false;
GLboolean move_left_flag = false;
GLboolean move_right_flag = false;
GLboolean debug_flag = false;
static bool f1_key_pressed = false;
std::vector<GraphicsSystem::GLViewport> GraphicsSystem::vps;

void WindowSystem::keyboardInputUpdateFlag() {
	left_turn_flag = glfwGetKey(GLFWFunctions::pWindow, GLFW_KEY_LEFT) != 0;
	right_turn_flag = glfwGetKey(GLFWFunctions::pWindow, GLFW_KEY_RIGHT) != 0;
	scale_up_flag = glfwGetKey(GLFWFunctions::pWindow, GLFW_KEY_UP) != 0;
	scale_down_flag = glfwGetKey(GLFWFunctions::pWindow, GLFW_KEY_DOWN) != 0;
	move_up_flag = glfwGetKey(GLFWFunctions::pWindow, GLFW_KEY_W) != 0;
	move_down_flag = glfwGetKey(GLFWFunctions::pWindow, GLFW_KEY_S) != 0;
	move_left_flag = glfwGetKey(GLFWFunctions::pWindow, GLFW_KEY_A) != 0;
	move_right_flag = glfwGetKey(GLFWFunctions::pWindow, GLFW_KEY_D) != 0;

	// Check if F1 is pressed and trigger the toggle only once
	if (glfwGetKey(GLFWFunctions::pWindow, GLFW_KEY_F1) == GLFW_PRESS) {
		if (!f1_key_pressed) {
			debug_flag = !debug_flag; // Toggle debug mode
			f1_key_pressed = true;    // Mark F1 as pressed
		}
	}
	else {
		f1_key_pressed = false; // Reset when F1 is released
	}
}


void WindowSystem::logicUpdate() {
	// Rotation logic
	if (left_turn_flag) {
		gameObject.orientation.y = 180.0f * GLFWFunctions::delta_time;
	}
	else if (right_turn_flag) {
		gameObject.orientation.y = -180.0f * GLFWFunctions::delta_time;
	}
	else {
		gameObject.orientation.y = 0.0f;  
	}

	// Scaling logic
	if (scale_up_flag) {
		if (gameObject.scaling.x < 5.0f && gameObject.scaling.y < 5.0f) {
			gameObject.scaling.x += 1.78f * GLFWFunctions::delta_time;
			gameObject.scaling.y += 1.0f * GLFWFunctions::delta_time;
		}
		graphicsSystem.SetCurrentAction(2);
	}
	else if (scale_down_flag) {
		if (gameObject.scaling.x > 0.1f && gameObject.scaling.y > 0.1f) {
			gameObject.scaling.x -= 1.78f * GLFWFunctions::delta_time;
			gameObject.scaling.y -= 1.0f * GLFWFunctions::delta_time;
		}
		graphicsSystem.SetCurrentAction(2);  // Action 2 for idle (third row)
	}

	// Movement logic
	bool isMoving = false;
	if (move_up_flag) {
		gameObject.position.y += 1.0f * GLFWFunctions::delta_time;
		isMoving = true;
		graphicsSystem.SetCurrentAction(0);  // Action 2 for idle (third row)
	}
	if (move_down_flag) {
		gameObject.position.y -= 1.0f * GLFWFunctions::delta_time;
		isMoving = true;
		graphicsSystem.SetCurrentAction(0);  // Action 2 for idle (third row)
	}
	if (move_left_flag) {
		gameObject.position.x -= 1.0f * GLFWFunctions::delta_time;
		isMoving = true;
		graphicsSystem.SetCurrentAction(1);  // Action 0 for move left (second row)
	}
	if (move_right_flag) {
		gameObject.position.x += 1.0f * GLFWFunctions::delta_time;
		isMoving = true;
		graphicsSystem.SetCurrentAction(3);  // Action 1 for move right (fourth row)
	}

	
	if (!isMoving) {
		graphicsSystem.SetCurrentAction(2);  // Action 2 for idle (third row)
	}

}




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
	

	graphicsSystem.Initialize();

	DebugTool->Initialise();

	gameObject.init(glm::vec2{ 0.0f, 0.0f }, glm::vec2{ 1.78f, 1.0f }, glm::vec2{ -0.5f, 0.0f });
	gameObject2.init(glm::vec2{ 0.0f, 0.0f }, glm::vec2{ 1.78f, 1.0f }, glm::vec2{ 0.5f, 0.0f });
	background.init(glm::vec2{ 0.0f, 0.0f }, glm::vec2{ 8, 2.0f }, glm::vec2{ 0.0f, 0.0f });
	blackBox.init(glm::vec2{ 0.0f, 0.0f }, glm::vec2{ 2.0f, 0.5f }, glm::vec2{ 0.0f, 0.0f });
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
	GraphicsSystem::vps.push_back({ 0, 0, 1400, 700 });// TODO::change this to be calculated based on the window size

	keyboardInputUpdateFlag();

	logicUpdate();

	DebugSystem::StartSystemTiming("Graphics"); 

	graphicsSystem.Update(GLFWFunctions::delta_time, false);
	graphicsSystem.Render(GLFWFunctions::delta_time);

	background.update(GLFWFunctions::delta_time);
	gameObject.update(GLFWFunctions::delta_time);
	gameObject2.update(GLFWFunctions::delta_time);
	blackBox.update(GLFWFunctions::delta_time);

	background.draw(shader, graphicsSystem.GetVAO(), graphicsSystem.GetTexture3());
	blackBox.draw(shader, graphicsSystem.GetVAO(), 0);
	// animation update
	graphicsSystem.Update(GLFWFunctions::delta_time, true);// TODO:: Check if object is animated and update accordingly
	gameObject.draw(shader, graphicsSystem.GetVAO(), graphicsSystem.GetTexture());
	gameObject2.draw(shader, graphicsSystem.GetVAO(), graphicsSystem.GetTexture2());

	if (debug_flag) {
		graphicsSystem.drawDebugLines(gameObject);
		graphicsSystem.drawDebugLines(gameObject2);
		gameObject.draw(shader, graphicsSystem.GetVAO(), graphicsSystem.GetTexture());
	}
	else {
		gameObject.draw(shader, graphicsSystem.GetVAO(), graphicsSystem.GetTexture());
	}
	if (debug_flag) {
		gameObject2.draw(shader, graphicsSystem.GetVAO(), graphicsSystem.GetTexture2());
	}
	else
		gameObject2.draw(shader, graphicsSystem.GetVAO(), graphicsSystem.GetTexture2());


	DebugSystem::EndSystemTiming("Graphics"); 

	DebugSystem::StartSystemTiming("Debug"); 
	DebugTool->Update();
	DebugSystem::EndSystemTiming("Debug"); 

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


