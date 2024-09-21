#pragma once

#include <vector>
#include "Systems.h"

//Engine for the entire game
class Engine
{
private:
	//Using vectors to store systems
	std::vector<Systems*> systems;
public:
	//public functions used by engine
	Engine();
	void addSystem(Systems* systems);
	void initialiseSystem();
	void updateSystem();
	void cleanupSystem();
	~Engine();
};
