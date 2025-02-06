/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   FilterBehaviour.h
@brief:  This header file includes the FilterBehaviour class which is used
		 by the logicSystemECS to handle the behaviour of the filter entities.
		 Joel Chu (c.weiyuan): declared the FilterBehaviour class and its member functions
							   and variables
							   100%
*//*___________________________________________________________________________-*/
#pragma once
#include "LogicSystemECS.h"

class FilterBehaviour : public BehaviourECS {
public:
	void update(Entity entity) override;
	void createCloggedAnimation(Entity entity);

private:
	bool isFilterUsed = false;
	std::chrono::time_point<std::chrono::steady_clock> collisionTime;
};