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

	void ApplyForce(Entity entity, const myMath::Vector2D& appliedForce);
	
	std::string getSystemECS() override;

private:

};