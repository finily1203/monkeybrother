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

	std::string getSystemECS() override;

private:
};