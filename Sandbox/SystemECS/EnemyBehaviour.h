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