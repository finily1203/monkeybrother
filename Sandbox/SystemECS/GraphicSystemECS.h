/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   GraphicSystemECS.h
@brief:  This header file inherits the System class from ECS base system class.
		 This class is used to handle the communication between ECS and graphic
		 system.
		 Joel Chu (c.weiyuan): Declared class GraphicSystemECS with its functions.
							   inherited from System class.
							   100%
*//*___________________________________________________________________________-*/
#pragma once
#include "EngineDefinitions.h"
#include "ECSCoordinator.h"
#include "EntityManager.h"
#include "GraphicsSystem.h"
#include "CameraSystem2D.h"
#include "LogicSystemECS.h"

class GraphicSystemECS : public System
{
public:
	GraphicSystemECS() = default;

	//Inherited functions from System class
	//Initialise and cleanup currently do not do anything
	void initialise() override;
	void cleanup() override;

	//Update function to update the graphics system
	//uses functions from GraphicsSystem class to update, draw
	//and render objects.
	void update(float dt) override;

	std::string getSystemECS() override;

private:
	MouseBehaviour mouseBehaviour;
	bool fpsEntityCreated = false;

};