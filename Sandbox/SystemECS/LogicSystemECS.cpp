#include "LogicSystemECS.h"
#include "GlobalCoordinator.h"

#include "Debug.h"
#include "GUIConsole.h"

void LogicSystemECS::initialise() {}

void LogicSystemECS::cleanup() {}

void LogicSystemECS::update(float dt) {

	//------------------------------------PLAYER MOVEMENT-----------------------------------//
	//Entity playerEntity = ecsCoordinator.getFirstEntity();
	Entity playerEntity = ecsCoordinator.getEntityFromID("player");

	const float maxSpeed = 0.6f;

	myMath::Vector2D& position =	ecsCoordinator.getComponent<TransformComponent>(playerEntity).position;
	myMath::Vector2D acceleration = ecsCoordinator.getComponent<RigidBodyComponent>(playerEntity).acceleration;
	myMath::Vector2D force =		ecsCoordinator.getComponent<RigidBodyComponent>(playerEntity).force;
	myMath::Vector2D& vel =			ecsCoordinator.getComponent<RigidBodyComponent>(playerEntity).velocity;
	myMath::Vector2D& accForce =	ecsCoordinator.getComponent<RigidBodyComponent>(playerEntity).accumulatedForce;
	float mass =					ecsCoordinator.getComponent<RigidBodyComponent>(playerEntity).mass;
	//float gravityScale =			ecsCoordinator.getComponent<RigidBodyComponent>(playerEntity).gravityScale;

	if (GLFWFunctions::move_left_flag) {
		ApplyForce(playerEntity, -force);
	}
	else if (GLFWFunctions::move_right_flag) {
		ApplyForce(playerEntity, force);
	}
	else {

		// Apply friction to gradually slow down
		if (accForce.GetX() > 0) {
			ApplyForce(playerEntity, -force);
		}
		else if (accForce.GetX() < 0) {
			ApplyForce(playerEntity, force);
		}

		if (std::abs(accForce.GetX()) < 0.01f) { // threshold
			vel.SetX(0.f);
			accForce.SetX(0.f);
		}

	}

	float invMass = mass > 0.f ? 1.f / mass : 0.f;
	acceleration = accForce * invMass;

	// Clamp speed without interfering with velocity
	if (vel.GetX() > maxSpeed) {
		acceleration.SetX(0);
	}
	else if (vel.GetX() < -maxSpeed) {
		acceleration.SetX(0);
	}

	vel.SetX(vel.GetX() + acceleration.GetX() * dt);
	vel.SetY(vel.GetY() + acceleration.GetY() * dt);

	// Dampening
	vel.SetX(vel.GetX() * 0.9f);

	position.SetX(position.GetX() + vel.GetX());
	position.SetY(position.GetY() + vel.GetY());

	//--------------------------------END OF PLAYER MOVEMENT--------------------------------//


	for (auto& entity : ecsCoordinator.getAllLiveEntities()) {
		auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
		
		//------------------------------------ENEMY MOVEMENT-----------------------------------//
		bool hasMovement = ecsCoordinator.hasComponent<RigidBodyComponent>(entity);
		bool hasEnemy = ecsCoordinator.hasComponent<EnemyComponent>(entity);

		if (hasMovement && hasEnemy) {
			myMath::Vector2D velocity = ecsCoordinator.getComponent<RigidBodyComponent>(entity).velocity;

			if (GLFWFunctions::left_turn_flag) {
				transform.orientation.SetY(transform.orientation.GetY() + (180.f * dt));
			}
			else if (GLFWFunctions::right_turn_flag) {
				transform.orientation.SetY(transform.orientation.GetY() - (180.0f * dt));
			}

			if (GLFWFunctions::scale_up_flag) {
				if (transform.scale.GetX() < 500.0f && transform.scale.GetY() < 500.0f) {
					transform.scale.SetX(transform.scale.GetX() + 53.4f * dt);
					transform.scale.SetY(transform.scale.GetY() + 30.0f * dt);
				}
			}
			else if (GLFWFunctions::scale_down_flag) {
				if (transform.scale.GetX() > 100.0f && transform.scale.GetY() > 100.0f) {
					transform.scale.SetX(transform.scale.GetX() - 53.4f * dt);
					transform.scale.SetY(transform.scale.GetY() - 30.0f * dt);
				}
			}

			myMath::Vector2D& ePos		   = ecsCoordinator.getComponent<TransformComponent>(entity).position;
			myMath::Vector2D eAcceleration = ecsCoordinator.getComponent<RigidBodyComponent>(entity).acceleration;
			myMath::Vector2D eForce		   = ecsCoordinator.getComponent<RigidBodyComponent>(entity).force;
			myMath::Vector2D& eVel		   = ecsCoordinator.getComponent<RigidBodyComponent>(entity).velocity;
			myMath::Vector2D& eAccForce	   = ecsCoordinator.getComponent<RigidBodyComponent>(entity).accumulatedForce;
			float eMass					   = ecsCoordinator.getComponent<RigidBodyComponent>(entity).mass;
			//float eGravityScale			   = ecsCoordinator.getComponent<RigidBodyComponent>(entity).gravityScale;

			if (GLFWFunctions::enemyMoveUp) {
				eForce.SetX(0.0f);
				ApplyForce(entity, eForce);
			}
			else if (GLFWFunctions::enemyMoveDown) {
				eForce.SetX(0.0f);
				ApplyForce(entity, -eForce);
			}
			else if (GLFWFunctions::enemyMoveLeft) {
				eForce.SetY(0.0f);
				ApplyForce(entity, -eForce);
			}
			else if (GLFWFunctions::enemyMoveRight) {
				eForce.SetY(0.0f);
				ApplyForce(entity, eForce);
			}
			else {

				// Apply friction to gradually slow down
				if (eAccForce.GetX() > 0) { 
					eForce.SetY(0.0f);
					ApplyForce(entity, -eForce);
				}
				else if (eAccForce.GetX() < 0) {
					eForce.SetY(0.0f);
					ApplyForce(entity, eForce);
				}
				else if (eAccForce.GetY() > 0) {
					eForce.SetX(0.0f);
					ApplyForce(entity, -eForce);
				}
				else if (eAccForce.GetY() < 0) {
					eForce.SetX(0.0f);
					ApplyForce(entity, eForce);
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

			invMass = eMass > 0.f ? 1.f / eMass : 0.f;
			eAcceleration = eAccForce * invMass;

			// Clamp speed without interfering with velocity
			if (eVel.GetX() > maxSpeed) {
				eAcceleration.SetX(0);
			}
			else if (eVel.GetX() < -maxSpeed) {
				eAcceleration.SetX(0);
			}

			eVel.SetX(eVel.GetX() + eAcceleration.GetX() * dt);
			eVel.SetY(eVel.GetY() + eAcceleration.GetY() * dt);

			// Dampening
			eVel.SetX(eVel.GetX() * 0.9f);
			eVel.SetY(eVel.GetY() * 0.9f);

			ePos.SetX(ePos.GetX() + eVel.GetX());
			ePos.SetY(ePos.GetY() + eVel.GetY());
		}
		//--------------------------------END OF ENEMY MOVEMENT--------------------------------//

	}
}

void LogicSystemECS::ApplyForce(Entity entity, const myMath::Vector2D& appliedForce) {
	myMath::Vector2D& accForce = ecsCoordinator.getComponent<RigidBodyComponent>(entity).accumulatedForce;

	accForce.SetX(accForce.GetX() + appliedForce.GetX());
	accForce.SetY(accForce.GetY() + appliedForce.GetY());

}

std::string LogicSystemECS::getSystemECS() {
	return "LogicSystemECS";
}
