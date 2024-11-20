/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   LogicSystemECS.cpp
@brief:  The source file includes all the implementation of the Logic for all entities.
		 Logic System is part of the ECS system, which handles the logic / scripts for
		 all entities in the game. Currently, it handles the jumping of player since our
		 current game demo is rotational based, there is no movement keys for the player.
		 Logic system also takes care of the enemy movement as well as camera movement.
		 Interacts with other ECS systems such as PhyColliSystemECS.

		 Joel Chu (c.weiyuan): Implemented all of the functions that belongs to
							   the LogicSystemECS.
							   100%
*//*___________________________________________________________________________-*/

#include "LogicSystemECS.h"
#include "GlobalCoordinator.h"
#include "PhyColliSystemECS.h"

#include "Debug.h"
#include "GUIConsole.h"

void LogicSystemECS::initialise() {}

void LogicSystemECS::cleanup() {}

//void LogicSystemECS::update(float dt) {
//
//	//------------------------------------PLAYER MOVEMENT-----------------------------------//
//	Entity playerEntity = ecsCoordinator.getEntityFromID("player");
//	auto PhysicsSystemRef = ecsCoordinator.getSpecificSystem<PhysicsSystemECS>();
//
//	const float maxSpeed = 0.6f;
//	float invMass;
//	Force playerForce =				 ecsCoordinator.getComponent<PhysicsComponent>(playerEntity).force;
//	myMath::Vector2D gravityScale =  ecsCoordinator.getComponent<PhysicsComponent>(playerEntity).gravityScale;
//	myMath::Vector2D& rotation =	 ecsCoordinator.getComponent<TransformComponent>(playerEntity).orientation;
//	float mag =						 playerForce.GetMagnitude();
//
//
//	if (GLFWFunctions::keyState[Key::Q]) {
//		rotation.SetX(rotation.GetX() + (180.f * dt));
//	}
//	else if (GLFWFunctions::keyState[Key::E]) {
//		rotation.SetX(rotation.GetX() - (180.f * dt));
//
//	}
//
//	if (PhysicsSystemRef->getIsColliding() && PhysicsSystemRef->GetAlrJumped()) {
//		if (GLFWFunctions::keyState[Key::SPACE]) {
//			PhysicsSystemRef->SetAlrJumped(false);  // Set jump state to prevent multiple jumps
//			PhysicsSystemRef->getForceManager().AddForce(playerEntity, myMath::Vector2D(-mag, -mag));
//		}
//	}
//
//	//--------------------------------END OF PLAYER MOVEMENT--------------------------------//
//
//
//	for (auto& entity : ecsCoordinator.getAllLiveEntities()) {
//		auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
//		
//		//------------------------------------ENEMY MOVEMENT-----------------------------------//
//		bool hasMovement = ecsCoordinator.hasComponent<PhysicsComponent>(entity);
//		bool hasEnemy = ecsCoordinator.hasComponent<EnemyComponent>(entity);
//
//		if (hasMovement && hasEnemy) {
//			myMath::Vector2D velocity = ecsCoordinator.getComponent<PhysicsComponent>(entity).velocity;
//
//			if (GLFWFunctions::keyState[Key::LEFT]) {
//				transform.orientation.SetY(transform.orientation.GetY() + (180.f * dt));
//			}
//			else if (GLFWFunctions::keyState[Key::RIGHT]) {
//				transform.orientation.SetY(transform.orientation.GetY() - (180.0f * dt));
//			}
//
//			if (GLFWFunctions::keyState[Key::UP]) {
//				if (transform.scale.GetX() < 500.0f && transform.scale.GetY() < 500.0f) {
//					transform.scale.SetX(transform.scale.GetX() + 53.4f * dt);
//					transform.scale.SetY(transform.scale.GetY() + 30.0f * dt);
//				}
//			}
//			else if (GLFWFunctions::keyState[Key::DOWN]) {
//				if (transform.scale.GetX() > 100.0f && transform.scale.GetY() > 100.0f) {
//					transform.scale.SetX(transform.scale.GetX() - 53.4f * dt);
//					transform.scale.SetY(transform.scale.GetY() - 30.0f * dt);
//				}
//			}
//
//			myMath::Vector2D& ePos		   = ecsCoordinator.getComponent<TransformComponent>(entity).position;
//			myMath::Vector2D eAcceleration = ecsCoordinator.getComponent<PhysicsComponent>(entity).acceleration;
//			//myMath::Vector2D eForce		   = ecsCoordinator.getComponent<PhysicsComponent>(entity).force;
//			myMath::Vector2D& eVel		   = ecsCoordinator.getComponent<PhysicsComponent>(entity).velocity;
//			myMath::Vector2D& eAccForce	   = ecsCoordinator.getComponent<PhysicsComponent>(entity).accumulatedForce;
//			float eMass					   = ecsCoordinator.getComponent<PhysicsComponent>(entity).mass;
//			//float eGravityScale			   = ecsCoordinator.getComponent<PhysicsComponent>(entity).gravityScale;
//			float eMagnitude				   = ecsCoordinator.getComponent<PhysicsComponent>(entity).force.GetMagnitude();
//
//			if (GLFWFunctions::keyState[Key::I]) {
//				PhysicsSystemRef->getForceManager().AddForce(entity, myMath::Vector2D(0.0f, eMagnitude));
//			}
//			else if (GLFWFunctions::keyState[Key::K]) {
//				PhysicsSystemRef->getForceManager().AddForce(entity, myMath::Vector2D(0.0f, -eMagnitude));
//			}
//			else if (GLFWFunctions::keyState[Key::J]) {
//				PhysicsSystemRef->getForceManager().AddForce(entity, myMath::Vector2D(-eMagnitude, 0.0f));
//			}
//			else if (GLFWFunctions::keyState[Key::L]) {
//				PhysicsSystemRef->getForceManager().AddForce(entity, myMath::Vector2D(eMagnitude, 0.0f));
//			}
//			else {
//
//				//Apply friction to gradually slow down
//				if (eAccForce.GetX() > 0) { 
//					PhysicsSystemRef->getForceManager().AddForce(entity, myMath::Vector2D(-eMagnitude, 0.0f));
//				}
//				else if (eAccForce.GetX() < 0) {
//					PhysicsSystemRef->getForceManager().AddForce(entity, myMath::Vector2D(eMagnitude, 0.0f));
//				}
//				else if (eAccForce.GetY() > 0) {
//					PhysicsSystemRef->getForceManager().AddForce(entity, myMath::Vector2D(0.0f, -eMagnitude));
//				}
//				else if (eAccForce.GetY() < 0) {
//					PhysicsSystemRef->getForceManager().AddForce(entity, myMath::Vector2D(0.0f, eMagnitude));
//				}
//
//				if (std::abs(eAccForce.GetX()) < 0.01f) { // threshold
//					eVel.SetX(0.f);
//					eAccForce.SetX(0.f);
//				}
//				if (std::abs(eAccForce.GetY()) < 0.01f) { // threshold
//					eVel.SetY(0.f);
//					eAccForce.SetY(0.f);
//				}
//			}
//
//			invMass = eMass > 0.f ? 1.f / eMass : 0.f;
//			eAcceleration = eAccForce * invMass;
//
//			// Clamp speed without interfering with velocity
//			if (eVel.GetX() > maxSpeed) {
//				eAcceleration.SetX(0);
//			}
//			else if (eVel.GetX() < -maxSpeed) {
//				eAcceleration.SetX(0);
//			}
//
//			eVel.SetX(eVel.GetX() + eAcceleration.GetX() * dt);
//			eVel.SetY(eVel.GetY() + eAcceleration.GetY() * dt);
//
//			eVel.SetX(eVel.GetX() * 0.9f);
//			eVel.SetY(eVel.GetY() * 0.9f);
//
//			ePos.SetX(ePos.GetX() + eVel.GetX());
//			ePos.SetY(ePos.GetY() + eVel.GetY());
//		}
//		//--------------------------------END OF ENEMY MOVEMENT--------------------------------//
//		//------------------------------------CAMERA MOVEMENT-----------------------------------//
//		auto& playerTransform = ecsCoordinator.getComponent<TransformComponent>(playerEntity);
//		cameraSystem.lockToComponent(playerTransform);
//
//		if (cameraSystem.checkLockedComponent() && (GLFWFunctions::allow_camera_movement == false)) {
//			if (GLFWFunctions::keyState[Key::Z])
//				cameraSystem.setCameraZoom(cameraSystem.getCameraZoom() + 0.1f * GLFWFunctions::delta_time);
//			if (GLFWFunctions::keyState[Key::X])
//				cameraSystem.setCameraZoom(cameraSystem.getCameraZoom() - 0.1f * GLFWFunctions::delta_time);
//		}
//		else {
//			myMath::Vector2D camPos = cameraSystem.getCameraPosition();
//			if (GLFWFunctions::keyState[Key::W]) {
//				camPos.SetY(camPos.GetY() + (20 * GLFWFunctions::delta_time));
//				cameraSystem.setCameraPosition(camPos);
//			}
//			if (GLFWFunctions::keyState[Key::S]) {
//				camPos.SetY(camPos.GetY() - (20 * GLFWFunctions::delta_time));
//				cameraSystem.setCameraPosition(camPos);
//			}
//			if (GLFWFunctions::keyState[Key::A]) {
//				camPos.SetX(camPos.GetX() - (20 * GLFWFunctions::delta_time));
//				cameraSystem.setCameraPosition(camPos);
//			}
//			if (GLFWFunctions::keyState[Key::D]) {
//				camPos.SetX(camPos.GetX() + (20 * GLFWFunctions::delta_time));
//				cameraSystem.setCameraPosition(camPos);
//			}
//
//			if (GLFWFunctions::keyState[Key::Z])
//				cameraSystem.setCameraZoom(cameraSystem.getCameraZoom() + 0.1f * GLFWFunctions::delta_time);
//			if (GLFWFunctions::keyState[Key::X])
//				cameraSystem.setCameraZoom(cameraSystem.getCameraZoom() - 0.1f * GLFWFunctions::delta_time);
//
//			if (GLFWFunctions::keyState[Key::Q]) {
//				cameraSystem.setCameraRotation(cameraSystem.getCameraRotation() + 0.1f * GLFWFunctions::delta_time);
//			}
//				
//			if (GLFWFunctions::keyState[Key::E]) {
//				cameraSystem.setCameraRotation(cameraSystem.getCameraRotation() - 0.1f * GLFWFunctions::delta_time);
//			}
//				
//		}
//		//--------------------------------END OF CAMERA MOVEMENT--------------------------------//
//	}
//}

void LogicSystemECS::update(float dt) {
	for (auto& [entity, behaviour] : behaviours) {
		behaviour->update(entity, dt);
	}
}

void LogicSystemECS::assignBehaviour(Entity entity, std::shared_ptr<BehaviourECS> behaviour) {
	behaviours[entity] = behaviour;
}

void PlayerBehaviour::update(Entity entity, float dt) {
	auto PhysicsSystemRef = ecsCoordinator.getSpecificSystem<PhysicsSystemECS>();

	Force playerForce = ecsCoordinator.getComponent<PhysicsComponent>(entity).force;
	myMath::Vector2D gravityScale = ecsCoordinator.getComponent<PhysicsComponent>(entity).gravityScale;
	myMath::Vector2D& rotation = ecsCoordinator.getComponent<TransformComponent>(entity).orientation;
	float mag = playerForce.GetMagnitude();


	if (GLFWFunctions::keyState[Key::Q]) {
		rotation.SetX(rotation.GetX() + (180.f * dt));
	}
	else if (GLFWFunctions::keyState[Key::E]) {
		rotation.SetX(rotation.GetX() - (180.f * dt));

	}

	if (PhysicsSystemRef->getIsColliding() && PhysicsSystemRef->GetAlrJumped()) {
		if (GLFWFunctions::keyState[Key::SPACE]) {
			PhysicsSystemRef->SetAlrJumped(false);  // Set jump state to prevent multiple jumps
			PhysicsSystemRef->getForceManager().AddForce(entity, myMath::Vector2D(-mag, -mag));
		}
	}

	//CAMERA BEHAVIOUR CURRENTLY PUT HERE FIRST SINCE IT IS AFFECTED BY PLAYER ENTITY
	auto& playerTransform = ecsCoordinator.getComponent<TransformComponent>(entity);
	cameraSystem.lockToComponent(playerTransform);

	if (cameraSystem.checkLockedComponent() && (GLFWFunctions::allow_camera_movement == false)) {
		if (GLFWFunctions::keyState[Key::Z])
			cameraSystem.setCameraZoom(cameraSystem.getCameraZoom() + 0.1f * GLFWFunctions::delta_time);
		if (GLFWFunctions::keyState[Key::X])
			cameraSystem.setCameraZoom(cameraSystem.getCameraZoom() - 0.1f * GLFWFunctions::delta_time);
	}
	else {
		myMath::Vector2D camPos = cameraSystem.getCameraPosition();
		if (GLFWFunctions::keyState[Key::W]) {
			camPos.SetY(camPos.GetY() + (20 * GLFWFunctions::delta_time));
			cameraSystem.setCameraPosition(camPos);
		}
		if (GLFWFunctions::keyState[Key::S]) {
			camPos.SetY(camPos.GetY() - (20 * GLFWFunctions::delta_time));
			cameraSystem.setCameraPosition(camPos);
		}
		if (GLFWFunctions::keyState[Key::A]) {
			camPos.SetX(camPos.GetX() - (20 * GLFWFunctions::delta_time));
			cameraSystem.setCameraPosition(camPos);
		}
		if (GLFWFunctions::keyState[Key::D]) {
			camPos.SetX(camPos.GetX() + (20 * GLFWFunctions::delta_time));
			cameraSystem.setCameraPosition(camPos);
		}

		if (GLFWFunctions::keyState[Key::Z])
			cameraSystem.setCameraZoom(cameraSystem.getCameraZoom() + 0.1f * GLFWFunctions::delta_time);
		if (GLFWFunctions::keyState[Key::X])
			cameraSystem.setCameraZoom(cameraSystem.getCameraZoom() - 0.1f * GLFWFunctions::delta_time);

		if (GLFWFunctions::keyState[Key::Q]) {
			cameraSystem.setCameraRotation(cameraSystem.getCameraRotation() + 0.1f * GLFWFunctions::delta_time);
		}

		if (GLFWFunctions::keyState[Key::E]) {
			cameraSystem.setCameraRotation(cameraSystem.getCameraRotation() - 0.1f * GLFWFunctions::delta_time);
		}

	}
}

void EnemyBehaviour::update(Entity entity, float dt) {
	auto PhysicsSystemRef = ecsCoordinator.getSpecificSystem<PhysicsSystemECS>();
	auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);

	myMath::Vector2D velocity = ecsCoordinator.getComponent<PhysicsComponent>(entity).velocity;

	if (GLFWFunctions::keyState[Key::LEFT]) {
		transform.orientation.SetY(transform.orientation.GetY() + (180.f * dt));
	}
	else if (GLFWFunctions::keyState[Key::RIGHT]) {
		transform.orientation.SetY(transform.orientation.GetY() - (180.0f * dt));
	}

	if (GLFWFunctions::keyState[Key::UP]) {
		if (transform.scale.GetX() < 500.0f && transform.scale.GetY() < 500.0f) {
			transform.scale.SetX(transform.scale.GetX() + 53.4f * dt);
			transform.scale.SetY(transform.scale.GetY() + 30.0f * dt);
		}
	}
	else if (GLFWFunctions::keyState[Key::DOWN]) {
		if (transform.scale.GetX() > 100.0f && transform.scale.GetY() > 100.0f) {
			transform.scale.SetX(transform.scale.GetX() - 53.4f * dt);
			transform.scale.SetY(transform.scale.GetY() - 30.0f * dt);
		}
	}

	myMath::Vector2D& ePos = ecsCoordinator.getComponent<TransformComponent>(entity).position;
	myMath::Vector2D eAcceleration = ecsCoordinator.getComponent<PhysicsComponent>(entity).acceleration;
	//myMath::Vector2D eForce		   = ecsCoordinator.getComponent<PhysicsComponent>(entity).force;
	myMath::Vector2D& eVel = ecsCoordinator.getComponent<PhysicsComponent>(entity).velocity;
	myMath::Vector2D& eAccForce = ecsCoordinator.getComponent<PhysicsComponent>(entity).accumulatedForce;
	float eMass = ecsCoordinator.getComponent<PhysicsComponent>(entity).mass;
	//float eGravityScale			   = ecsCoordinator.getComponent<PhysicsComponent>(entity).gravityScale;
	float eMagnitude = ecsCoordinator.getComponent<PhysicsComponent>(entity).force.GetMagnitude();

	if (GLFWFunctions::keyState[Key::I]) {
		PhysicsSystemRef->getForceManager().AddForce(entity, myMath::Vector2D(0.0f, eMagnitude));
	}
	else if (GLFWFunctions::keyState[Key::K]) {
		PhysicsSystemRef->getForceManager().AddForce(entity, myMath::Vector2D(0.0f, -eMagnitude));
	}
	else if (GLFWFunctions::keyState[Key::J]) {
		PhysicsSystemRef->getForceManager().AddForce(entity, myMath::Vector2D(-eMagnitude, 0.0f));
	}
	else if (GLFWFunctions::keyState[Key::L]) {
		PhysicsSystemRef->getForceManager().AddForce(entity, myMath::Vector2D(eMagnitude, 0.0f));
	}
	else {

		//Apply friction to gradually slow down
		if (eAccForce.GetX() > 0) {
			PhysicsSystemRef->getForceManager().AddForce(entity, myMath::Vector2D(-eMagnitude, 0.0f));
		}
		else if (eAccForce.GetX() < 0) {
			PhysicsSystemRef->getForceManager().AddForce(entity, myMath::Vector2D(eMagnitude, 0.0f));
		}
		else if (eAccForce.GetY() > 0) {
			PhysicsSystemRef->getForceManager().AddForce(entity, myMath::Vector2D(0.0f, -eMagnitude));
		}
		else if (eAccForce.GetY() < 0) {
			PhysicsSystemRef->getForceManager().AddForce(entity, myMath::Vector2D(0.0f, eMagnitude));
		}

		if (std::abs(eAccForce.GetX()) < 0.01f) { // threshold
			eVel.SetX(0.f);
			eAccForce.SetX(0.f);
		}
		if (std::abs(eAccForce.GetY()) < 0.01f) { // threshold
			eVel.SetY(0.f);
			eAccForce.SetY(0.f);
		}
	}

	float invMass = eMass > 0.f ? 1.f / eMass : 0.f;
	eAcceleration = eAccForce * invMass;
	const float maxSpeed = 0.6f;

	// Clamp speed without interfering with velocity
	if (eVel.GetX() > maxSpeed) {
		eAcceleration.SetX(0);
	}
	else if (eVel.GetX() < -maxSpeed) {
		eAcceleration.SetX(0);
	}

	eVel.SetX(eVel.GetX() + eAcceleration.GetX() * dt);
	eVel.SetY(eVel.GetY() + eAcceleration.GetY() * dt);

	eVel.SetX(eVel.GetX() * 0.9f);
	eVel.SetY(eVel.GetY() * 0.9f);

	ePos.SetX(ePos.GetX() + eVel.GetX());
	ePos.SetY(ePos.GetY() + eVel.GetY());
}

//camera behaviour should only be affected by one entity
//void CameraBehaviour::update(Entity entity, float dt) {
//
//}

void LogicSystemECS::ApplyForce(Entity entity, const myMath::Vector2D& appliedForce) {
	myMath::Vector2D& accForce = ecsCoordinator.getComponent<PhysicsComponent>(entity).accumulatedForce;

	accForce.SetX(accForce.GetX() + appliedForce.GetX());
	accForce.SetY(accForce.GetY() + appliedForce.GetY());

}

std::string LogicSystemECS::getSystemECS() {
	return "LogicSystemECS";
}

