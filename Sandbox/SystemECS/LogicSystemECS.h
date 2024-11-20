/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   LogicSystemECS.h
@brief:  This header file inherits the System class from ECS base system class.
		 This class is used to handle the logic that happens for each entity
		 in the game. Currently logic systems handle player, enemy and camera
		 Joel Chu (c.weiyuan): Declared class LogicSystemECS with its functions.
							   inherited from System class.
							   100%
*//*___________________________________________________________________________-*/
#pragma once
#include "ECSCoordinator.h"
#include "GraphicsSystem.h"


class BehaviourECS {
public:
	virtual ~BehaviourECS() = default;
	virtual void update(Entity entity, float dt) = 0;
};

class PlayerBehaviour : public BehaviourECS {
public:
	void update(Entity entity, float dt) override;
};

class EnemyBehaviour : public BehaviourECS {
public:
	void update(Entity entity, float dt) override;
};

//class CameraBehaviour : public BehaviourECS {
//public:
//	void update(Entity entity, float dt) override;
//};

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
	//But might need to expand this in the future
	void assignBehaviour(Entity entity, std::shared_ptr<BehaviourECS> behaviour);

	void ApplyForce(Entity entity, const myMath::Vector2D& appliedForce);
	
	std::string getSystemECS() override;

private:
	std::unordered_map<Entity, std::shared_ptr<BehaviourECS>> behaviours;
};