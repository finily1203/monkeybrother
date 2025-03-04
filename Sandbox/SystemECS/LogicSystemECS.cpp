/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan), Ian Loi (ian.loi)
@team:   MonkeHood
@course: CSD2401
@file:   LogicSystemECS.cpp
@brief:  The source file includes all the implementation of the Logic for all entities.
		 Logic System is part of the ECS system, which handles the logic / scripts for
		 all entities in the game. Currently, it handles the jumping of player since our
		 current game demo is rotational based, there is no movement keys for the player.
		 Logic system also takes care of the enemy movement as well as camera movement.
		 Interacts with other ECS systems such as PhyColliSystemECS. Logic system now also
		 handles Behaviour logic like mouse on-click and on-hover behaviour.

		 Joel Chu (c.weiyuan): Implemented all of the functions that belongs to
							   the LogicSystemECS.
							   90%
		 Ian Loi  (ian.loi)  : Implemented functions that belongs to the MouseBehaviour
							   class which inherits from BehaviourECS.
							   10%
*//*___________________________________________________________________________-*/

#include "LogicSystemECS.h"
#include "GlobalCoordinator.h"
#include "PhyColliSystemECS.h"

#include "Debug.h"
#include "GUIConsole.h"

void LogicSystemECS::initialise() {}

void LogicSystemECS::cleanup() {
	behaviours.clear();
}


void LogicSystemECS::update(float dt) {
	//for each entity, update the behaviour
	for (auto& entity : ecsCoordinator.getAllLiveEntities()) {
		if (behaviours.find(entity) != behaviours.end()) {
			behaviours[entity]->update(entity);
		}
	}

	(void)dt;

}

void LogicSystemECS::assignBehaviour(Entity entity, std::shared_ptr<BehaviourECS> behaviour) {
	behaviours[entity] = behaviour;
}

void LogicSystemECS::unassignBehaviour(Entity entity) {
	behaviours.erase(entity);
}


void MouseBehaviour::update(Entity entity) {
	if (glfwGetWindowAttrib(GLFWFunctions::pWindow, GLFW_HOVERED))
	{
		double mouseX{}, mouseY{};
		int windowWidth{}, windowHeight{};
		glfwGetCursorPos(GLFWFunctions::pWindow, &mouseX, &mouseY);
		glfwGetWindowSize(GLFWFunctions::pWindow, &windowWidth, &windowHeight);

		float cursorXCentered = static_cast<float>(mouseX) - (windowWidth / 2.f);
		float cursorYCentered = (windowHeight / 2.f) - static_cast<float>(mouseY);

		if (!GLFWFunctions::debug_flag)
		{
			onMouseHover(static_cast<double>(cursorXCentered), static_cast<double>(cursorYCentered));
		}
	}

	(void)entity;
}

void MouseBehaviour::onMouseClick(GLFWwindow* window, double mouseX, double mouseY)
{
	auto allEntities = ecsCoordinator.getAllLiveEntities();

	for (auto& entity : allEntities)
	{
		if (ecsCoordinator.hasComponent<ButtonComponent>(entity))
		{
			TransformComponent& transform = ecsCoordinator.getComponent<TransformComponent>(entity);

			if (mouseIsOverButton(mouseX, mouseY, transform))
			{
				handleButtonClick(window, entity);
			}
		}
	}
}

void MouseBehaviour::onMouseHover(double mouseX, double mouseY)
{
	auto allEntities = ecsCoordinator.getAllLiveEntities();

	for (auto& entity : allEntities)
	{
		if (ecsCoordinator.hasComponent<ButtonComponent>(entity))
		{
			TransformComponent& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
			ButtonComponent& button = ecsCoordinator.getComponent<ButtonComponent>(entity);

			if (mouseIsOverButton(mouseX, mouseY, transform))
			{
				transform.scale.SetX(button.hoveredScale.GetX());
				transform.scale.SetY(button.hoveredScale.GetY());
			}

			else
			{
				transform.scale.SetX(button.originalScale.GetX());
				transform.scale.SetY(button.originalScale.GetY());
			}
		}
	}
}

bool MouseBehaviour::mouseIsOverButton(double mouseX, double mouseY, TransformComponent& transform)
{
	float const scalar = 0.7f;
    float buttonLeft = transform.position.GetX() - transform.scale.GetX() * scalar / 2.f;
    float buttonRight = transform.position.GetX() + transform.scale.GetX() * scalar / 2.f;
    float buttonTop = transform.position.GetY() + transform.scale.GetY() * scalar / 2.f;
    float buttonBottom = transform.position.GetY() - transform.scale.GetY() * scalar / 2.f;

    return (mouseX >= static_cast<double>(buttonLeft) && mouseX <= static_cast<double>(buttonRight) && mouseY >= static_cast<double>(buttonBottom) && mouseY <= static_cast<double>(buttonTop));
}

void MouseBehaviour::handleButtonClick(GLFWwindow* window, Entity entity)
{
	std::string entityId = ecsCoordinator.getEntityID(entity);
	auto allEntities = ecsCoordinator.getAllLiveEntities();

	if (entityId == "quitButton")
	{
		audioSystem.playSoundEffect("UI_ButtonClick.wav");

		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}

	else if (entityId == "retryButton")
	{
		for (auto currEntity : allEntities)
		{
			ecsCoordinator.destroyEntity(currEntity);
		}

		audioSystem.playSoundEffect("UI_ButtonClick.wav");

		ecsCoordinator.test5();
	}
}

void LogicSystemECS::ApplyForce(Entity entity, const myMath::Vector2D& appliedForce) {
	myMath::Vector2D& accForce = ecsCoordinator.getComponent<PhysicsComponent>(entity).accumulatedForce;

	accForce.SetX(accForce.GetX() + appliedForce.GetX());
	accForce.SetY(accForce.GetY() + appliedForce.GetY());

}

std::string LogicSystemECS::getSystemECS() {
	return "LogicSystemECS";
}

