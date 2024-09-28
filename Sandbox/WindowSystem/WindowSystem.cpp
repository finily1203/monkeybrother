#include <GraphicsSystem.h>
#include "Debug.h"
#include "WindowSystem.h"
#include <iostream>


GraphicsSystem graphicsSystem;
DebugSystem* DebugTool = new DebugSystem();
GraphicsSystem::GLObject gameObject, gameObject2;
Shader *shader = nullptr;
GLboolean left_turn_flag = false;
GLboolean right_turn_flag = false;
GLboolean scale_up_flag = false;
GLboolean scale_down_flag = false;
GLboolean move_up_flag = false;
GLboolean move_down_flag = false;
GLboolean move_left_flag = false;
GLboolean move_right_flag = false;
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
}

void WindowSystem::logicUpdate() {
	if (left_turn_flag) {
		gameObject.orientation.y = 1.0f;
	}
	else if (right_turn_flag) {
		gameObject.orientation.y = -1.0f;
	}
	else {
		gameObject.orientation.y = 0.0f;
	}
	
	if (scale_up_flag) {
		if (gameObject.scaling.x < 5.0f && gameObject.scaling.y < 5.0f) {
			gameObject.scaling.x += 0.178f;
			gameObject.scaling.y += 0.1f;
		}
	}
	else if (scale_down_flag) {
		if (gameObject.scaling.x > 0.1f && gameObject.scaling.y > 0.1f) {
			gameObject.scaling.x -= 0.178f;
			gameObject.scaling.y -= 0.1f;
		}
	}

	if (move_up_flag) {
		gameObject.position.y += 1 * GLFWFunctions::delta_time;
	}
	else if (move_down_flag) {
		gameObject.position.y -= 1 * GLFWFunctions::delta_time;
	}
	if (move_left_flag) {
		gameObject.position.x -= 1 * GLFWFunctions::delta_time;
	}
	else if (move_right_flag) {
		gameObject.position.x += 1 * GLFWFunctions::delta_time;
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
	// Set the clear colorglClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	graphicsSystem.Initialize();

	DebugTool->Initialise();

	gameObject.init(glm::vec2{ 0.0f, 0.0f }, glm::vec2{ 1.78f, 1.0f }, glm::vec2{ -0.5f, 0.0f });
	gameObject2.init(glm::vec2{ 0.0f, 0.0f }, glm::vec2{ 1.78f, 1.0f }, glm::vec2{ 0.5f, 0.0f });

	shader = graphicsSystem.GetShader();
}

void WindowSystem::update() {
	
	glfwPollEvents();

	if (glfwWindowShouldClose(GLFWFunctions::pWindow)) {
		glfwSetWindowShouldClose(GLFWFunctions::pWindow, GLFW_TRUE);
	}
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	keyboardInputUpdateFlag();

	logicUpdate();

	DebugSystem::StartSystemTiming("Graphics"); //Get start of graphics gameloop
	graphicsSystem.Update(GLFWFunctions::delta_time);
	graphicsSystem.Render(GLFWFunctions::delta_time);
	gameObject.update(GLFWFunctions::delta_time);
	gameObject2.update(GLFWFunctions::delta_time);
	gameObject.draw(shader, graphicsSystem);
	gameObject2.draw(shader, graphicsSystem);
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


