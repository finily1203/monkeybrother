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

float PlayerBehaviour::MOVEMENT_THRESHOLD = 0;      // Default values
float PlayerBehaviour::BASE_ROTATION_SPEED = 0;     // Will be overwritten 
float PlayerBehaviour::MAX_ROTATION_PER_FRAME = 0;  // during initialization
float PlayerBehaviour::ROTATION_SPEED;
float PlayerBehaviour::HOLD_TIME = 0.f;
float PlayerBehaviour::ACCELERATION = 0.f;

void PlayerBehaviour::update(Entity entity) {
	auto PhysicsSystemRef = ecsCoordinator.getSpecificSystem<PhysicsSystemECS>();
	cameraSystem.readGameplaySettingsFromJSON(FilePathManager::GetGameplaySettingsJSONPath());
	ROTATION_SPEED = GLFWFunctions::rotationSpeed;
	const float timeThreshold = 3.f;
	const float accelerationRate = 10.f;

	Force playerForce = ecsCoordinator.getComponent<PhysicsComponent>(entity).force;
	ForceManager forceManager = ecsCoordinator.getComponent<PhysicsComponent>(entity).forceManager;
	myMath::Vector2D gravityScale = ecsCoordinator.getComponent<PhysicsComponent>(entity).gravityScale;
	myMath::Vector2D& rotation = ecsCoordinator.getComponent<TransformComponent>(entity).orientation;
	float mag = playerForce.GetMagnitude();

	//auto& rotation = ecsCoordinator.getComponent<TransformComponent>(entity).orientation;

	// Toggle between mouse and keyboard control when T is pressed
	static bool wasPressed = false;
	if ((*GLFWFunctions::keyState)[Key::T]) {
		if (!wasPressed) {
			GLFWFunctions::useMouseRotation = !GLFWFunctions::useMouseRotation;
			GLFWFunctions::updateCursorState(); // Update cursor state when toggling
			wasPressed = true;
		}
	}
	else {
		wasPressed = false;
	}

	if (GLFWFunctions::useMouseRotation) {
		// Mouse rotation logic
		double mouseMovement = GLFWFunctions::mouseXDelta;
		if (std::abs(mouseMovement) > MOVEMENT_THRESHOLD) {
			float rotationAmount = static_cast<float>(mouseMovement) * BASE_ROTATION_SPEED;
			rotationAmount *= (60.0f * GLFWFunctions::delta_time);
			rotationAmount = std::clamp(rotationAmount, -MAX_ROTATION_PER_FRAME, MAX_ROTATION_PER_FRAME);

			rotation.SetX(rotation.GetX() + rotationAmount);
		}
	}
	else {
		// Keyboard rotation logic
		if ((*GLFWFunctions::keyState)[Key::D]) {
			HOLD_TIME += GLFWFunctions::delta_time;

			if (HOLD_TIME >= timeThreshold)
			{
				ACCELERATION += accelerationRate * GLFWFunctions::delta_time;
				ROTATION_SPEED += ACCELERATION;
			}

			rotation.SetX(rotation.GetX() + (ROTATION_SPEED * GLFWFunctions::delta_time));
		}

		else if ((*GLFWFunctions::keyState)[Key::A]) {
			HOLD_TIME += GLFWFunctions::delta_time;

			if (HOLD_TIME >= timeThreshold)
			{
				ACCELERATION += accelerationRate * GLFWFunctions::delta_time;
				ROTATION_SPEED += ACCELERATION;
			}

			rotation.SetX(rotation.GetX() - (ROTATION_SPEED * GLFWFunctions::delta_time));
		}

		else
		{
			HOLD_TIME = 0.f;
			//ACCELERATION = 0.f;

			if (ACCELERATION > 0.f)
			{
				ACCELERATION -= accelerationRate * GLFWFunctions::delta_time;
				ACCELERATION = std::max(ACCELERATION, 0.f);
			}
		}
	}

	// Reset mouse delta regardless of control mode
	GLFWFunctions::mouseXDelta = 0.0;

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