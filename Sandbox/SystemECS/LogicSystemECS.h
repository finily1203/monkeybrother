/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan), Ian Loi (ian.loi)
@team:   MonkeHood
@course: CSD2401
@file:   LogicSystemECS.h
@brief:  This header file inherits the System class from ECS base system class.
		 This class is used to handle the logic that happens for each entity
		 in the game. Currently logic systems handle player, enemy and camera
		 Joel Chu (c.weiyuan): Declared class LogicSystemECS with its functions.
							   inherited from System class.
							   90%
		 Ian Loi  (ian.loi)  : Declared class MouseBehaviour that inherits from
							   BehaviourECS class.
							   10%
*//*___________________________________________________________________________-*/
#pragma once
#include "ECSCoordinator.h"
#include "GraphicsSystem.h"


class BehaviourECS {
public:
	virtual ~BehaviourECS() = default;
	virtual void update(Entity entity) = 0;
};

class MouseBehaviour : public BehaviourECS {
public:
	void update(Entity entity) override;
	void onMouseClick(GLFWwindow* window, double mouseX, double mouseY);
	void onMouseHover(double mouseX, double mouseY);

private:
	bool mouseIsOverButton(double mouseX, double mouseY, TransformComponent& transform);
	void handleButtonClick(GLFWwindow* window, Entity entity);
};

class LogicSystemECS : public System
{
public:
	LogicSystemECS() = default;

	//Inherited functions from System class
	//Initialise and cleanup currently do not do anything
	void initialise() override;
	void cleanup() override;

	//Update function to update the graphics system
	//uses functions from GraphicsSystem class to update, draw
	//and render objects.
	void update(float dt) override;

	//For now we don't use a behaviour manager since we have little behaviours
	//But need to expand this in the future
	void assignBehaviour(Entity entity, std::shared_ptr<BehaviourECS> behaviour);

	void ApplyForce(Entity entity, const myMath::Vector2D& appliedForce);
	
	std::string getSystemECS() override;

private:
	std::unordered_map<Entity, std::shared_ptr<BehaviourECS>> behaviours;
};