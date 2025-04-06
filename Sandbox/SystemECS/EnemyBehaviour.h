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

	//void drawDebugVisionCone(Entity entity, myMath::Matrix3x3 viewMatrix);

	void update(Entity entity) override;
	void switchState(STATE newState);
	std::vector<myMath::Vector2D>& getWaypoints();
	int& getCurrentWaypointIndex();

	void updatePatrolState(Entity entity);

	bool doesEnemySeePlayer(Entity entity, Entity playerEntity);
	bool isWallBlockingVision(myMath::Vector2D enemyPos, myMath::Vector2D playerPos);
	bool rayIntersectAABB(myMath::Vector2D rayOrigin, myMath::Vector2D rayDir, myMath::Vector2D aabbMin, myMath::Vector2D aabbMax, float& tmin, float& tmax);
	bool AABBIntersect(const myMath::Vector2D& min1, const myMath::Vector2D& max1, const myMath::Vector2D& min2, const myMath::Vector2D& max2);

	void updateChaseState(Entity entity);
	bool checkPlayerCollision(Entity entity, Entity playerEntity);

	void updateAttackState(Entity entity);

	void createChaseAnimation(Entity entity);
	void createAttackAnimation(Entity entity, Entity playerEntity);

private:
	STATE currentState;
	std::vector<myMath::Vector2D> waypoints;
	int currentWaypointIndex = 0;
	bool isFacingRight;
	bool moveHorizontal;
	bool hasWaypointsBeenChanged;
	bool isAvoidingWalls;
	float avoidTimer;
	bool chaseAnimationCreated;
	bool attackAnimationCreated;
	int timesAvoided;
};