/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   PlayerBehaviour.cpp
@brief:  This source file includes the implementation of the PlayerBehaviour
		 that logicSystemECS uses to handle the behaviour of the player entity.
		 Note that the camera behaviour is also included as part of the player
		 since in our game the camera is locked onto the player entity unless
		 told otherwise. This code is similar to what we implemented in LogicSystemECS
		 in Milestone 2.

		 Joel Chu (c.weiyuan): defined the functions of PlayerBehaviour class
							   100%
*//*___________________________________________________________________________-*/

#include "PlayerBehaviour.h"
#include "GlobalCoordinator.h"
#include "PhyColliSystemECS.h"

void PlayerBehaviour::update(Entity entity) {
	auto PhysicsSystemRef = ecsCoordinator.getSpecificSystem<PhysicsSystemECS>();

	Force playerForce = ecsCoordinator.getComponent<PhysicsComponent>(entity).force;
	ForceManager forceManager = ecsCoordinator.getComponent<PhysicsComponent>(entity).forceManager;
	myMath::Vector2D gravityScale = ecsCoordinator.getComponent<PhysicsComponent>(entity).gravityScale;
	myMath::Vector2D& rotation = ecsCoordinator.getComponent<TransformComponent>(entity).orientation;
	float mag = playerForce.GetMagnitude();


	if ((*GLFWFunctions::keyState)[Key::D]) {
		rotation.SetX(rotation.GetX() + (180.f * GLFWFunctions::delta_time));
	}
	else if ((*GLFWFunctions::keyState)[Key::A]) {
		rotation.SetX(rotation.GetX() - (180.f * GLFWFunctions::delta_time));

	}

	if (PhysicsSystemRef->getIsColliding() && PhysicsSystemRef->GetAlrJumped()) {
		if ((*GLFWFunctions::keyState)[Key::SPACE]) {
			PhysicsSystemRef->SetAlrJumped(false);  // Set jump state to prevent multiple jumps
			forceManager.AddForce(entity, myMath::Vector2D(-mag, -mag));
		}
	}

	//CAMERA BEHAVIOUR CURRENTLY PUT HERE FIRST SINCE IT IS AFFECTED BY PLAYER ENTITY
	auto& playerTransform = ecsCoordinator.getComponent<TransformComponent>(entity);
	cameraSystem.lockToComponent(playerTransform);

	if (cameraSystem.checkLockedComponent() && (GLFWFunctions::allow_camera_movement == false)) {
		if ((*GLFWFunctions::keyState)[Key::Z])
			cameraSystem.setCameraZoom(cameraSystem.getCameraZoom() + 0.1f * GLFWFunctions::delta_time);
		if ((*GLFWFunctions::keyState)[Key::X])
			cameraSystem.setCameraZoom(cameraSystem.getCameraZoom() - 0.1f * GLFWFunctions::delta_time);
	}
	else {
		myMath::Vector2D camPos = cameraSystem.getCameraPosition();
		if ((*GLFWFunctions::keyState)[Key::I]) {
			camPos.SetY(camPos.GetY() + (20 * GLFWFunctions::delta_time));
			cameraSystem.setCameraPosition(camPos);
		}
		if ((*GLFWFunctions::keyState)[Key::K]) {
			camPos.SetY(camPos.GetY() - (20 * GLFWFunctions::delta_time));
			cameraSystem.setCameraPosition(camPos);
		}
		if ((*GLFWFunctions::keyState)[Key::J]) {
			camPos.SetX(camPos.GetX() - (20 * GLFWFunctions::delta_time));
			cameraSystem.setCameraPosition(camPos);
		}
		if ((*GLFWFunctions::keyState)[Key::L]) {
			camPos.SetX(camPos.GetX() + (20 * GLFWFunctions::delta_time));
			cameraSystem.setCameraPosition(camPos);
		}

		if ((*GLFWFunctions::keyState)[Key::Z])
			cameraSystem.setCameraZoom(cameraSystem.getCameraZoom() + 0.1f * GLFWFunctions::delta_time);
		if ((*GLFWFunctions::keyState)[Key::X])
			cameraSystem.setCameraZoom(cameraSystem.getCameraZoom() - 0.1f * GLFWFunctions::delta_time);

		if ((*GLFWFunctions::keyState)[Key::D]) {
			cameraSystem.setCameraRotation(cameraSystem.getCameraRotation() + 0.1f * GLFWFunctions::delta_time);
		}

		if ((*GLFWFunctions::keyState)[Key::A]) {
			cameraSystem.setCameraRotation(cameraSystem.getCameraRotation() - 0.1f * GLFWFunctions::delta_time);
		}

	}
}