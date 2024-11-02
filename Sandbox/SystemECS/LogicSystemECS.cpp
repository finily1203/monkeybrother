#include "LogicSystemECS.h"
#include "GlobalCoordinator.h"

#include "Debug.h"
#include "GUIConsole.h"

void LogicSystemECS::initialise() {}

void LogicSystemECS::cleanup() {}

void LogicSystemECS::update(float dt) {\

	//------------------------------------PLAYER MOVEMENT-----------------------------------//
	Entity playerEntity = ecsCoordinator.getFirstEntity();
	playerEntity = ecsCoordinator.getEntityFromID("player");

	const float maxSpeed = 0.6f;

	myMath::Vector2D& playerPos = ecsCoordinator.getComponent<TransformComponent>(playerEntity).position;
	myMath::Vector2D acceleration = ecsCoordinator.getComponent<RigidBodyComponent>(playerEntity).acceleration;
	myMath::Vector2D& force = ecsCoordinator.getComponent<RigidBodyComponent>(playerEntity).force;
	myMath::Vector2D& vel = ecsCoordinator.getComponent<RigidBodyComponent>(playerEntity).velocity;
	float mass = ecsCoordinator.getComponent<RigidBodyComponent>(playerEntity).mass;
	float gravityScale = ecsCoordinator.getComponent<RigidBodyComponent>(playerEntity).gravityScale;

	if (GLFWFunctions::move_left_flag) {
		ApplyForce(playerEntity, myMath::Vector2D(-0.05f, 0.f));
	}
	else if (GLFWFunctions::move_right_flag) {
		ApplyForce(playerEntity, myMath::Vector2D(0.05f, 0.f));
	}
	else {

		// Apply friction to gradually slow down
		if (force.GetX() > 0) {
			ApplyForce(playerEntity, myMath::Vector2D(-0.05f, 0.f));
		}
		else if (force.GetX() < 0) {
			ApplyForce(playerEntity, myMath::Vector2D(0.05f, 0.f));
		}

		if (std::abs(force.GetX()) < 0.01f) { // threshold
			vel.SetX(0.f);
			force.SetX(0.f);
		}

	}

	float invMass = mass > 0.f ? 1.f / mass : 0.f;
	acceleration = force * invMass;

	// Clamp speed without interfering with velocity
	if (vel.GetX() > maxSpeed) {
		acceleration.SetX(0);
	}
	else if (vel.GetX() < -maxSpeed) {
		acceleration.SetX(0);
	}

	vel.SetX(vel.GetX() + acceleration.GetX() * GLFWFunctions::delta_time);
	vel.SetY(vel.GetY() + acceleration.GetY() * GLFWFunctions::delta_time);

	// Dampening
	vel.SetX(vel.GetX() * 0.9f);

	playerPos.SetX(playerPos.GetX() + vel.GetX() * GLFWFunctions::delta_time);
	playerPos.SetY(playerPos.GetY() + vel.GetY() * GLFWFunctions::delta_time);

	myMath::Vector2D velocity = ecsCoordinator.getComponent<RigidBodyComponent>(playerEntity).velocity;

	//--------------------------------END OF PLAYER MOVEMENT--------------------------------//


	for (auto& entity : ecsCoordinator.getAllLiveEntities()) {
		auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
		
		//------------------------------------ENEMY MOVEMENT-----------------------------------//
		bool hasMovement = ecsCoordinator.hasComponent<RigidBodyComponent>(entity);
		bool hasEnemy = ecsCoordinator.hasComponent<EnemyComponent>(entity);

		if (hasMovement && hasEnemy) {
			myMath::Vector2D velocity = ecsCoordinator.getComponent<RigidBodyComponent>(entity).velocity;

			if (GLFWFunctions::left_turn_flag) {
				transform.orientation.SetY(transform.orientation.GetY() + (180.f * GLFWFunctions::delta_time));
			}
			else if (GLFWFunctions::right_turn_flag) {
				transform.orientation.SetY(transform.orientation.GetY() - (180.0f * GLFWFunctions::delta_time));
			}

			if (GLFWFunctions::scale_up_flag) {
				if (transform.scale.GetX() < 500.0f && transform.scale.GetY() < 500.0f) {
					transform.scale.SetX(transform.scale.GetX() + 53.4f * GLFWFunctions::delta_time);
					transform.scale.SetY(transform.scale.GetY() + 30.0f * GLFWFunctions::delta_time);
				}
			}
			else if (GLFWFunctions::scale_down_flag) {
				if (transform.scale.GetX() > 100.0f && transform.scale.GetY() > 100.0f) {
					transform.scale.SetX(transform.scale.GetX() - 53.4f * GLFWFunctions::delta_time);
					transform.scale.SetY(transform.scale.GetY() - 30.0f * GLFWFunctions::delta_time);
				}
			}

			if (GLFWFunctions::enemyMoveUp) {
				transform.position.SetY(transform.position.GetY() + velocity.GetY() * GLFWFunctions::delta_time);
			}
			if (GLFWFunctions::enemyMoveDown) {
				transform.position.SetY(transform.position.GetY() - velocity.GetY() * GLFWFunctions::delta_time);
			}
			if (GLFWFunctions::enemyMoveLeft) {
				transform.position.SetX(transform.position.GetX() - velocity.GetX() * GLFWFunctions::delta_time);
			}
			if (GLFWFunctions::enemyMoveRight) {
				transform.position.SetX(transform.position.GetX() + velocity.GetX() * GLFWFunctions::delta_time);
			}
		}
		//--------------------------------END OF ENEMY MOVEMENT--------------------------------//

	}
}

void LogicSystemECS::ApplyForce(Entity player, const myMath::Vector2D& appliedForce) {
	myMath::Vector2D& force = ecsCoordinator.getComponent<RigidBodyComponent>(player).force;

	force.SetX(force.GetX() + appliedForce.GetX());
	force.SetY(force.GetY() + appliedForce.GetY());

}

std::string LogicSystemECS::getSystemECS() {
	return "LogicSystemECS";
}
