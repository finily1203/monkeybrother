#pragma once
//Systems used for the game

//Forward declaration for perfomance viewer to avoid circular dependencies
enum SystemType; 

class GameSystems
{
public:
	virtual void initialise() = 0;
	virtual void update() = 0;
	virtual void cleanup() = 0;
	virtual SystemType getSystem() = 0; //For perfomance viewer
	virtual ~GameSystems() = default;
};