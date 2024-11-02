#include "LogicSystemECS.h"
#include "GlobalCoordinator.h"

#include "Debug.h"
#include "GUIConsole.h"

void LogicSystemECS::initialise() {}

void LogicSystemECS::cleanup() {}

void LogicSystemECS::update(float dt) {

	for (auto& entity : ecsCoordinator.getAllLiveEntities()) {
		auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
		
		//------------------------------------ENEMY MOVEMENT-----------------------------------//
		bool hasMovement = ecsCoordinator.hasComponent<RigidBodyComponent>(entity);
		bool hasEnemy = ecsCoordinator.hasComponent<EnemyComponent>(entity);

		if (hasMovement && hasEnemy) {
			float speed = ecsCoordinator.getComponent<RigidBodyComponent>(entity).speed;

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
				transform.position.SetY(transform.position.GetY() + speed * GLFWFunctions::delta_time);
			}
			if (GLFWFunctions::enemyMoveDown) {
				transform.position.SetY(transform.position.GetY() - speed * GLFWFunctions::delta_time);
			}
			if (GLFWFunctions::enemyMoveLeft) {
				transform.position.SetX(transform.position.GetX() - speed * GLFWFunctions::delta_time);
			}
			if (GLFWFunctions::enemyMoveRight) {
				transform.position.SetX(transform.position.GetX() + speed * GLFWFunctions::delta_time);
			}
		}
		//--------------------------------END OF ENEMY MOVEMENT--------------------------------//

		//------------------------------------PLAYER MOVEMENT-----------------------------------//
		 
		 
	
		//--------------------------------END OF PLAYER MOVEMENT--------------------------------//
	}
}

std::string LogicSystemECS::getSystemECS() {
	return "LogicSystemECS";
}
