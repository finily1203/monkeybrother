/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   EnemyBehaviour.h
@brief:  This header file includes the EnemyBehaviour class which is used
		 by the logicSystemECS to handle the behaviour of the enemy entities.
		 Note that in the enum there are 3 states. However, only the patrol state
		 is implemented. The other states will be defined in future implementations.

		 Joel Chu (c.weiyuan): declared the EnemyBehaviour class
							   100%
*//*___________________________________________________________________________-*/

#pragma once
#include "LogicSystemECS.h"


class EnemyBehaviour : public BehaviourECS {
public:
	enum STATE {
		PATROL,
		CHASE,
		ATTACK
	};

	EnemyBehaviour();
	~EnemyBehaviour() {
		waypoints.clear();
		std::vector<myMath::Vector2D>().swap(waypoints);
	}

	void update(Entity entity) override;
	void switchState(STATE newState);
	std::vector<myMath::Vector2D>& getWaypoints();
	int& getCurrentWaypointIndex();

	void updatePatrolState(Entity entity);

private:
	STATE currentState;
	std::vector<myMath::Vector2D> waypoints;
	int currentWaypointIndex = 0;
};