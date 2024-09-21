#include <GL/glew.h> //To include glew, must include it before glfw3.h
#include "GlfwFunctions.h"
#include <iostream>
#include "Engine.h"
#include "WindowSystem.h"
#include "TestEntityAndComponent.h"


namespace monkeybrother {
	__declspec(dllimport) void Print();
}

int main() {
	monkeybrother::Print();

	Engine* engine = new Engine();
	//EntityManager entityManager;
	//entityManager.testEntityManager();

	//testComponentManager();

	//TestEntityAndComponent();

	WindowSystem* windowSystem = new WindowSystem();
	engine->addSystem(windowSystem);

	engine->initialiseSystem();
	while (!glfwWindowShouldClose(GLFWFunctions::pWindow)) {
		engine->updateSystem();
	}
	engine->cleanupSystem();
	delete engine;

	return 0;
}




//ECS - Entity Component System
//
//Entity->The unique ID of an object
//Component->Data only classes, structs that contains the use of the component.
//Example:
//class Entity
//{
//    unsigned int idTag;
//    std::vector<Component*> Components;
//}
//
//struct Component
//{
//
//}
//
//struct TransformComponent : public component
//{
//    vec4 transformMatrix;
//    Mesh* mesh;
//}
//
//struct GravityComponent : public component
//{
//    float speed;
//    vec3 direction;
//}
//
//System->Essentially a similar system like update or load but contains only component based scripts
//Example : GravitySystem / PhysicsSystem->uses both TransformComponent and GravityComponent
//
//When rendering a scene, during the update, instead of check per entity, check per component
//If the entity that shares this component is within the scene, it would perform an action or smth