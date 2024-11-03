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
#include "EnemyComponent.h"

#include "GlobalCoordinator.h"
#include "GraphicsSystem.h"
#include "Debug.h"
#include "GUIConsole.h"
#include "vector"

CameraSystem2D cameraSystem;
std::vector<GraphicsSystem::GLViewport> vps;

std::unique_ptr<EntityManager> entityManager;
//Initialise currently does not do anything
void GraphicSystemECS::initialise() {
	cameraSystem.initialise();
	vps.push_back({ 0, 0, GLFWFunctions::windowWidth, GLFWFunctions::windowHeight });
	glViewport(vps[0].x, vps[0].y, vps[0].width, vps[0].height);
}

//Update function to update the graphics system
//uses functions from GraphicsSystem class to update, draw
//and render objects.
void GraphicSystemECS::update(float dt) {
	Shader* shader = assetsManager.GetShader("shader1");
	Shader* shader2 = assetsManager.GetShader("shader2");


	for (auto entity : entities) {
		//check if entity has transform component
		auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
		//Console::GetLog() << "Entity: " << entity << " Position: " << transform.position.x << ", " << transform.position.y << std::endl;
		Console::GetLog() << "Entity: " << entity << " Position: " << transform.position.GetX() << ", " << transform.position.GetY() << std::endl;

		//auto& graphics = ecsCoordinator.getComponent<GraphicsComponent>(entity);
		//Console::GetLog() << "Entity: " << entity << " Position (GLObj): " << graphics.glObject.position.x << ", " << graphics.glObject.position.y << std::endl;

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
			bool hasEnemy = ecsCoordinator.hasComponent<EnemyComponent>(entity);
			//if (hasMovement && isEnemy) {
			if (hasMovement && hasEnemy) {
				float speed = ecsCoordinator.getComponent<MovementComponent>(entity).speed;

				if (GLFWFunctions::left_turn_flag) {
					//transform.orientation.y = 180.0f * GLFWFunctions::delta_time;
					transform.orientation.SetY(transform.orientation.GetY() + (180.f * GLFWFunctions::delta_time));
					//graphics.glObject.orientation.y = transform.orientation.y;
				}
				else if (GLFWFunctions::right_turn_flag) {
					//transform.orientation.y = -180.0f * GLFWFunctions::delta_time;
					transform.orientation.SetY(transform.orientation.GetY() - (180.0f * GLFWFunctions::delta_time));
					//graphics.glObject.orientation.y = transform.orientation.y;
				}
				else {
					//transform.orientation.y = 0.0f;
					//transform.orientation.SetY(0.0f);
					//graphics.glObject.orientation.y = transform.orientation.y;
				}

				// Scaling logic
				if (GLFWFunctions::scale_up_flag) {
					//if (transform.scale.x < 5.0f && transform.scale.y < 5.0f) {
					//	transform.scale.x += 1.78f * GLFWFunctions::delta_time;
					//	transform.scale.y += 1.0f * GLFWFunctions::delta_time;
					//	//graphics.glObject.scaling = transform.scale;
					//}					
					if (transform.scale.GetX() < 500.0f && transform.scale.GetY() < 500.0f) {
						transform.scale.SetX(transform.scale.GetX() + 53.4f * GLFWFunctions::delta_time);
						transform.scale.SetY(transform.scale.GetY() + 30.0f * GLFWFunctions::delta_time);
						//graphics.glObject.scaling = transform.scale;
					}
				}
				else if (GLFWFunctions::scale_down_flag) {
					//if (transform.scale.x > 0.1f && transform.scale.y > 0.1f) {
					//	transform.scale.x -= 1.78f * GLFWFunctions::delta_time;
					//	transform.scale.y -= 1.0f * GLFWFunctions::delta_time;
					//	//graphics.glObject.scaling = transform.scale;
					//}
					if (transform.scale.GetX() > 100.0f && transform.scale.GetY() > 100.0f) {
						transform.scale.SetX(transform.scale.GetX() - 53.4f * GLFWFunctions::delta_time);
						transform.scale.SetY(transform.scale.GetY() - 30.0f * GLFWFunctions::delta_time);
						//graphics.glObject.scaling = transform.scale;
					}
				}

				// Movement logic
				if (GLFWFunctions::enemyMoveUp) {
					//transform.position.y += 1.0f * GLFWFunctions::delta_time;
					transform.position.SetY(transform.position.GetY() + speed * GLFWFunctions::delta_time);
					//graphics.glObject.position = transform.position;
				}
				if (GLFWFunctions::enemyMoveDown) {
					//transform.position.y -= 1.0f * GLFWFunctions::delta_time;
					transform.position.SetY(transform.position.GetY() - speed * GLFWFunctions::delta_time);
					//graphics.glObject.position = transform.position;
				}
				if (GLFWFunctions::enemyMoveLeft) {
					//transform.position.x -= 1.0f * GLFWFunctions::delta_time;
					transform.position.SetX(transform.position.GetX() - speed * GLFWFunctions::delta_time);
					//graphics.glObject.position = transform.position;
				}
				if (GLFWFunctions::enemyMoveRight) {
					//transform.position.x += 1.0f * GLFWFunctions::delta_time;
					transform.position.SetX(transform.position.GetX() + speed * GLFWFunctions::delta_time);
					//graphics.glObject.position = transform.position;
				}

				//graphics.glObject.position = transform.position;
			}

			graphicsSystem.Update(GLFWFunctions::delta_time / 10, true);
			transform.mdl_xform = graphicsSystem.UpdateObject(GLFWFunctions::delta_time, transform.position, transform.scale, transform.orientation, cameraSystem.getViewMatrix());
			

			auto entitySig = ecsCoordinator.getEntitySignature(entity);

			// compute view matrix
			if (GLFWFunctions::allow_camera_movement) { // Press F2 to allow camera movement
				cameraSystem.update();
			}
			else if (ecsCoordinator.getEntityID(entity) == "player") {
				cameraSystem.lockToComponent(transform);
				cameraSystem.update();
			}

			// TODO:: Update AABB component inside game loop
			// Press F1 to draw out debug AABB
			if (GLFWFunctions::debug_flag) {
				graphicsSystem.drawDebugAABB(ecsCoordinator.getComponent<AABBComponent>(entity), cameraSystem.getViewMatrix());
			}

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
			// compute view matrix
			if (GLFWFunctions::allow_camera_movement) { // Press F2 to allow camera movement
				cameraSystem.update();
			}
			else if (ecsCoordinator.getEntityID(entity) == "Player") {
				cameraSystem.lockToComponent(transform);
				cameraSystem.update();
			}
			
			// It updates the object based on the animation component
			graphicsSystem.Update(GLFWFunctions::delta_time / 10, true);
			// calculate the model transform matrix using update object function
			transform.mdl_xform = graphicsSystem.UpdateObject(GLFWFunctions::delta_time, transform.position, transform.scale, transform.orientation
																, cameraSystem.getViewMatrix());
			
			// Draw the object
			// TODO:: change getTexture to some enum or something
			if (GLFWFunctions::debug_flag) {
				graphicsSystem.drawDebugAABB(ecsCoordinator.getComponent<AABBComponent>(entity), cameraSystem.getViewMatrix());
			}
			graphicsSystem.DrawObject(GraphicsSystem::DrawMode::TEXTURE, assetsManager.GetTexture("texture1"), transform.mdl_xform);
		}

	}
}

//cleanup currently does not do anything
void GraphicSystemECS::cleanup() {}

std::string GraphicSystemECS::getSystemECS() {
	return "GraphicsSystemECS";
}