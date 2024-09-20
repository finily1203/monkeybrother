#pragma once

//Systems used for the game
class Systems
{
public:
	virtual void initialise() = 0;
	virtual void update() = 0;
	virtual void cleanup() = 0;
	virtual ~Systems() = default;
};