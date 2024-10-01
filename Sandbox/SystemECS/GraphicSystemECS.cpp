#include "ECSCoordinator.h"
#include "GraphicSystemECS.h"
#include "TransformComponent.h"
#include "GraphicsComponent.h"
#include "GlobalCoordinator.h"
#include "GraphicsSystem.h"

void GraphicSystemECS::initialise() {}

void GraphicSystemECS::update(float dt) {
	Shader* shader = graphicsSystem.GetShader();

	for (auto entity : entities) {
		//check if entity has transform component

		auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
		std::cout << "Entity: " << entity << " Position: " << transform.position.x << ", " << transform.position.y << std::endl;
		//std::cout << "Entity: " << entity << " Scale: " << transform.scale.x << ", " << transform.scale.y << std::endl;
		//std::cout << "Entity: " << entity << " Rotation: " << transform.orientation.x << ", " << transform.orientation.y << std::endl;

		auto& graphics = ecsCoordinator.getComponent<GraphicsComponent>(entity);

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

		graphics.glObject.draw(shader, graphicsSystem.GetVAO(), 0);
	}
}

void GraphicSystemECS::cleanup(){}