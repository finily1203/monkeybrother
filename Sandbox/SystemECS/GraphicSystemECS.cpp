/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   GraphicSystemECS.cpp
@brief:  This source file defines the functions in GraphicSystemECS class.
		 This class is used to handle the communication between ECS and graphic
		 system.
		 Joel Chu (c.weiyuan): Defined the functions in GraphicSystemECS class.
							   Most of the content in this file is taken from
							   WindowSystem.cpp and modified to fit the scope
							   for creating object in ECS.
							   100%
*//*___________________________________________________________________________-*/
#include "GraphicSystemECS.h"
#include "TransformComponent.h"
#include "GraphicsComponent.h"
#include "AABBComponent.h"
#include "MovementComponent.h"
#include "AnimationComponent.h"
#include "EnemyComponent.h"
#include "RigidBodyComponent.h"
#include "GlobalCoordinator.h"
#include "GraphicsSystem.h"
#include "Debug.h"
#include "GUIConsole.h"


//Initialise currently does not do anything
void GraphicSystemECS::initialise() {}

//Update function to update the graphics system
//uses functions from GraphicsSystem class to update, draw
//and render objects.
void GraphicSystemECS::update(float dt) {
	Shader* shader = assetsManager.GetShader("shader1");
	Shader* shader2 = assetsManager.GetShader("shader2");

	for (auto entity : entities) {
		//check if entity has transform component
		auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
		Console::GetLog() << "Entity: " << entity << " Position: " << transform.position.GetX() << ", " << transform.position.GetY() << std::endl;

		//check if entity has animation component
		auto& animation = ecsCoordinator.getComponent<AnimationComponent>(entity);
        Console::GetLog() << "Entity: " << entity << " Animation: " << (animation.isAnimated ? "True" : "False") << std::endl;

		if (GLFWFunctions::testMode == 0) {
			bool hasMovement = ecsCoordinator.hasComponent<RigidBodyComponent>(entity);
			bool hasEnemy = ecsCoordinator.hasComponent<EnemyComponent>(entity);

			graphicsSystem.Update(GLFWFunctions::delta_time / 10, true);
			transform.mdl_xform = graphicsSystem.UpdateObject(GLFWFunctions::delta_time, transform.position, transform.scale, transform.orientation);

			auto entitySig = ecsCoordinator.getEntitySignature(entity);

			if (hasMovement && hasEnemy) 
			{
				graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("texture2"), transform.mdl_xform);
			}
			else if (hasMovement) 
			{
				graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("texture1"), transform.mdl_xform);
			}
			else if (entitySig.test(0) && entitySig.count() == 1) {
				graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("texture1"), transform.mdl_xform);
			}
			else 
			{
				graphicsSystem.DrawObject(GraphicsSystem::DrawMode::COLOR, 0, transform.mdl_xform);
			}

		}
		
		else if (GLFWFunctions::testMode == 1) {
			// It updates the object based on the animation component
			graphicsSystem.Update(GLFWFunctions::delta_time / 10, true);
			// calculate the model transform matrix using update object function
			transform.mdl_xform = graphicsSystem.UpdateObject(GLFWFunctions::delta_time, transform.position, transform.scale, transform.orientation);
			
			// Draw the object
			// TODO:: change getTexture to some enum or something
			graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("texture1"), transform.mdl_xform);
		}

	}
}

//cleanup currently does not do anything
void GraphicSystemECS::cleanup() {}

std::string GraphicSystemECS::getSystemECS() {
	return "GraphicsSystemECS";
}