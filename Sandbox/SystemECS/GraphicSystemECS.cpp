#include "ECSCoordinator.h"
#include "GraphicSystemECS.h"
#include "TransformComponent.h"
#include "GlobalCoordinator.h"
#include "GraphicsSystem.h"

void GraphicSystemECS::initialise() {}

void GraphicSystemECS::update(float dt) {
	for (auto entity : entities) {
		//check if entity has transform component

		auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
		//std::cout << "Entity: " << entity << " Position: " << transform.position.x << ", " << transform.position.y << std::endl;
		//std::cout << "Entity: " << entity << " Scale: " << transform.scale.x << ", " << transform.scale.y << std::endl;
		//std::cout << "Entity: " << entity << " Rotation: " << transform.orientation.x << ", " << transform.orientation.y << std::endl;

		if (GLFWFunctions::left_turn_flag) {
			transform.orientation.y = 180.0f * GLFWFunctions::delta_time;
		}
		else if (GLFWFunctions::right_turn_flag) {
			transform.orientation.y = -180.0f * GLFWFunctions::delta_time;
		}
		else {
			transform.orientation.y = 0.0f;
		}

		// Scaling logic
		if (GLFWFunctions::scale_up_flag) {
			if (transform.scale.x < 5.0f && transform.scale.y < 5.0f) {
				transform.scale.x += 1.78f * GLFWFunctions::delta_time;
				transform.scale.y += 1.0f * GLFWFunctions::delta_time;
			}
			m_graphicsSystem->SetCurrentAction(2);
		}
		else if (GLFWFunctions::scale_down_flag) {
			if (transform.scale.x > 0.1f && transform.scale.y > 0.1f) {
				transform.scale.x -= 1.78f * GLFWFunctions::delta_time;
				transform.scale.y -= 1.0f * GLFWFunctions::delta_time;
			}
			m_graphicsSystem->SetCurrentAction(2);  // Action 2 for idle (third row)
		}

		// Movement logic
		if (GLFWFunctions::move_up_flag) {
			transform.position.y += 1.0f * GLFWFunctions::delta_time;
		}
		if (GLFWFunctions::move_down_flag) {
			transform.position.y -= 1.0f * GLFWFunctions::delta_time;
		}
		if (GLFWFunctions::move_left_flag) {
			transform.position.x -= 1.0f * GLFWFunctions::delta_time;
		}
		if (GLFWFunctions::move_right_flag) {
			transform.position.x += 1.0f * GLFWFunctions::delta_time;
		}
	}
}

void GraphicSystemECS::cleanup(){}