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
#include "ECSCoordinator.h"
#include "GraphicSystemECS.h"
#include "TransformComponent.h"
#include "GraphicsComponent.h"
#include "AABBComponent.h"
#include "MovementComponent.h"
#include "AnimationComponent.h"
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
		Console::GetLog() << "Entity: " << entity << " Position: " << transform.position.x << ", " << transform.position.y << std::endl;

		auto& graphics = ecsCoordinator.getComponent<GraphicsComponent>(entity);
		Console::GetLog() << "Entity: " << entity << " Position (GLObj): " << graphics.glObject.position.x << ", " << graphics.glObject.position.y << std::endl;

		//check if entity has animation component
		auto& animation = ecsCoordinator.getComponent<AnimationComponent>(entity);
        Console::GetLog() << "Entity: " << entity << " Animation: " << (animation.isAnimated ? "True" : "False") << std::endl;

		if (GLFWFunctions::testMode == 0) {
			//If user presses clone button ("C"), clones "player object"
			if (GLFWFunctions::cloneObject) {
				ecsCoordinator.cloneEntity(entity);
				GLFWFunctions::cloneObject = false;
			}

			//Taken from WindowSystem.cpp
			bool hasMovement = ecsCoordinator.hasComponent<MovementComponent>(entity);
			if (hasMovement) {
				if (GLFWFunctions::left_turn_flag) {
					transform.orientation.y = 180.0f * GLFWFunctions::delta_time;
					graphics.glObject.orientation.y = transform.orientation.y;
				}
				else if (GLFWFunctions::right_turn_flag) {
					transform.orientation.y = -180.0f * GLFWFunctions::delta_time;
					graphics.glObject.orientation.y = transform.orientation.y;
				}
				else {
					transform.orientation.y = 0.0f;
					graphics.glObject.orientation.y = transform.orientation.y;
				}

				// Scaling logic
				if (GLFWFunctions::scale_up_flag) {
					if (transform.scale.x < 5.0f && transform.scale.y < 5.0f) {
						transform.scale.x += 1.78f * GLFWFunctions::delta_time;
						transform.scale.y += 1.0f * GLFWFunctions::delta_time;
						graphics.glObject.scaling = transform.scale;
					}
				}
				else if (GLFWFunctions::scale_down_flag) {
					if (transform.scale.x > 0.1f && transform.scale.y > 0.1f) {
						transform.scale.x -= 1.78f * GLFWFunctions::delta_time;
						transform.scale.y -= 1.0f * GLFWFunctions::delta_time;
						graphics.glObject.scaling = transform.scale;
					}
				}

				// Movement logic
				if (GLFWFunctions::move_up_flag) {
					transform.position.y += 1.0f * GLFWFunctions::delta_time;
					graphics.glObject.position = transform.position;
				}
				if (GLFWFunctions::move_down_flag) {
					transform.position.y -= 1.0f * GLFWFunctions::delta_time;
					graphics.glObject.position = transform.position;
				}
				if (GLFWFunctions::move_left_flag) {
					transform.position.x -= 1.0f * GLFWFunctions::delta_time;
					graphics.glObject.position = transform.position;
				}
				if (GLFWFunctions::move_right_flag) {
					transform.position.x += 1.0f * GLFWFunctions::delta_time;
					graphics.glObject.position = transform.position;
				}

				graphics.glObject.position = transform.position;
			}

			//Update object and draw based on test mode.
			graphics.glObject.update(GLFWFunctions::delta_time);
			if (GLFWFunctions::testMode == 0)
			{
				graphics.glObject.draw(shader2, graphicsSystem.GetVAO(), assetsManager.GetTexture("texture1"));
			}
			else if (GLFWFunctions::testMode == 1)
			{
				graphics.glObject.draw(shader, graphicsSystem.GetVAO(), 0);
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