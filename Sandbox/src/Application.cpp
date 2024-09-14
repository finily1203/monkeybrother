#include <GL/glew.h> //To include glew, must include it before glfw3.h
#include "GlfwFunctions.h"
#include <iostream>


namespace monkeybrother {
	__declspec(dllimport) void Print();
}

static void init();
static void update();
static void draw();
static void cleanup();

int main() {
	monkeybrother::Print();
    init();

    while (!glfwWindowShouldClose(GLFWFunctions::pWindow))
    {
		update();
		draw();
	}

    cleanup();
}

static void init() {
    GLFWFunctions::init(1600, 900, "Hello World");
}

static void update() {
    /* Loop until the user closes the window */

    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT);

    /* Swap front and back buffers */
    glfwSwapBuffers(GLFWFunctions::pWindow);

    /* Poll for and process events */
    glfwPollEvents();

    GLFWFunctions::showFPS(GLFWFunctions::pWindow);
    glfwSetWindowTitle(GLFWFunctions::pWindow, std::to_string(GLFWFunctions::fps).c_str());
}

static void draw() {

}

static void cleanup() {
	GLFWFunctions::glfwCleanup();
}