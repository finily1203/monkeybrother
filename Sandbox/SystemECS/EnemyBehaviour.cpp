/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   EnemyBehaviour.cpp
@brief:  This source file includes the implementation of the EnemyBehaviour
         that logicSystemECS uses to handle the behaviour of the Enemy entities.
         Note that in the enum there are 3 states. However, only the patrol state
         is implemented. The other states will be defined in future implementations.
         Waypoints are also current set to a fixed path for all enemies.

         Joel Chu (c.weiyuan): defined the functions of EnemyBehaviour class
                               100%
*//*___________________________________________________________________________-*/

#include "EnemyBehaviour.h"
#include "GlobalCoordinator.h"
#include "PhyColliSystemECS.h"

EnemyBehaviour::EnemyBehaviour() {
	currentState = PATROL;
    isFacingRight = true;
	moveHorizontal = true;

	//For now all enemies have same way point
    
    //Test CW
    waypoints.push_back(myMath::Vector2D(-200, 50));
    //waypoints.push_back(myMath::Vector2D(200, 200));
    waypoints.push_back(myMath::Vector2D(300, 100));
    waypoints.push_back(myMath::Vector2D(-300, -100));

    waypoints.push_back(myMath::Vector2D(-100, 0));
    waypoints.push_back(myMath::Vector2D(-200, -200));
    waypoints.push_back(myMath::Vector2D(-200, -100));
    waypoints.push_back(myMath::Vector2D(-100, -100));
    waypoints.push_back(myMath::Vector2D(-100, 100));
    waypoints.push_back(myMath::Vector2D(-200, 100));

}

void EnemyBehaviour::switchState(STATE newState) {  
   currentState = newState;  
}

void EnemyBehaviour::update(Entity entity) {
    auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
	myMath::Vector2D velocity = ecsCoordinator.getComponent<PhysicsComponent>(entity).velocity;

	//updateEntityRotation(entity, velocity);

    auto playerEntity = ecsCoordinator.getEntityFromID("player");
	bool enemySeePlayer = doesEnemySeePlayer(entity, playerEntity);
	if (enemySeePlayer) {
		switchState(CHASE);
        std::cout << "Enemy Sees Player" << std::endl;
	}

	switch (currentState) {
	case PATROL:
		std::cout << "moving to waypoint " << currentWaypointIndex << std::endl;
		updatePatrolState(entity);
		break;
	case CHASE:
        updateChaseState(entity);
		break;
	case ATTACK:
		break;
	}
}

//void EnemyBehaviour::updateEntityRotation(Entity entity, myMath::Vector2D velocity) {
//    auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
//	auto& clockwise = ecsCoordinator.getComponent<EnemyComponent>(entity).isClockwise;
//
//  //  // Only update rotation if there's meaningful movement
//  //  const float minVelocityThreshold = 0.01f;
//  //  float velocityMagnitude = static_cast<float>(std::sqrt(std::pow(velocity.GetX(), 2) + std::pow(velocity.GetY(), 2)));
//
//  //  if (velocityMagnitude > minVelocityThreshold) {
//  //      // Handle horizontal orientation (flipping)
//  //      if (velocity.GetX() > minVelocityThreshold && !EnemyBehaviour::isFacingRight) {
//  //          // Fish is moving right but facing left, so flip it
//  //          transform.scale.SetX(std::abs(transform.scale.GetX())); // Make scale positive
//		//	EnemyBehaviour::isFacingRight = true;
//  //      }
//  //      else if (velocity.GetX() < -minVelocityThreshold && EnemyBehaviour::isFacingRight) {
//  //          // Fish is moving left but facing right, so flip it
//  //          transform.scale.SetX(-std::abs(transform.scale.GetX())); // Make scale negative
//		//	EnemyBehaviour::isFacingRight = false;
//  //      }
//
//  //      // Calculate proper rotation for up/down movement
//  //      float rotationAngle = 0.0f;
//
//  //      if (std::abs(velocity.GetY()) > minVelocityThreshold) {
//		//	//std::cout << velocity.GetY() << std::endl;
//  //          float yDirection = 0.f;
//  //          if (velocity.GetY() < 0.f) {
//  //              if (clockwise) { yDirection = velocity.GetY(); }
//  //              else { yDirection = -velocity.GetY(); }
//		//	}
//  //          else {
//  //              if (clockwise) { yDirection = -velocity.GetY(); }
//  //              else { yDirection = velocity.GetY(); }
//  //          }
//
//  //          float xComponent = isFacingRight ? std::abs(velocity.GetX()) : -std::abs(velocity.GetX());
//
//  //          // Only calculate rotation if horizontal movement isn't dominant
//  //          if (std::abs(velocity.GetY()) > 0.5f * std::abs(velocity.GetX())) {
//  //              if (clockwise) {
//  //                  if (velocity.GetY() > 0.f && isFacingRight) {
//  //                      rotationAngle = static_cast<float>(std::atan2(yDirection, xComponent) * -(180.0 / 3.14159265358979323846));
//  //                  }
//  //                  else {
//  //                      rotationAngle = static_cast<float>(std::atan2(yDirection, xComponent) * (180.0 / 3.14159265358979323846));
//  //                  }
//  //              }
//  //              else {
//  //                  if (velocity.GetY() < 0.f && isFacingRight) {
//		//				rotationAngle = static_cast<float>(std::atan2(yDirection, xComponent) * -(180.0 / 3.14159265358979323846));
//		//			}
//  //                  else {
//  //                      rotationAngle = static_cast<float>(std::atan2(yDirection, xComponent) * (180.0 / 3.14159265358979323846));
//  //                  }
//  //              }
//
//  //              // Clamp the rotation to avoid extreme angles (-30° to 30°)
//  //              rotationAngle = std::max(-90.f, std::min(rotationAngle, 90.f));
//  //          }
//  //      }
//
//  //      // Apply rotation
//		//transform.orientation.SetX(rotationAngle);
//		//
//  //  }
//}

// ==================================== PATROL STATE IMPLEMENTATION ==================================== //

std::vector<myMath::Vector2D>& EnemyBehaviour::getWaypoints() {
	return waypoints;
}

int& EnemyBehaviour::getCurrentWaypointIndex() {
	return currentWaypointIndex;
}

void EnemyBehaviour::updatePatrolState(Entity entity) {
    auto PhysicsSystemRef = ecsCoordinator.getSpecificSystem<PhysicsSystemECS>();
    auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
    auto& physics = ecsCoordinator.getComponent<PhysicsComponent>(entity);
    auto& currentWaypoints = getWaypoints();
    int& currentWPIndex = getCurrentWaypointIndex();
    Force force = ecsCoordinator.getComponent<PhysicsComponent>(entity).force;
    auto& forceManager = ecsCoordinator.getComponent<PhysicsComponent>(entity).forceManager;
    myMath::Vector2D gravity = ecsCoordinator.getComponent<PhysicsComponent>(entity).gravityScale;
    float mass = ecsCoordinator.getComponent<PhysicsComponent>(entity).mass;

    if (currentWaypoints.empty())
    {
        return; // No waypoints to follow
    }

    Console::GetLog() << "Waypoints: " << currentWaypoints.size() << std::endl;
    Console::GetLog() << "Current waypoint: " << currentWPIndex << std::endl;

    myMath::Vector2D targetWP = currentWaypoints[currentWPIndex];
    myMath::Vector2D direction = targetWP - transform.position;

    // Compute squared distance (avoid using .Length())
    float squaredDist = direction.GetX() * direction.GetX() + direction.GetY() * direction.GetY();
    float threshold = 100.0f; // Squared threshold (avoid sqrt)

    // Move toward waypoint
    float length = sqrt(squaredDist); // Compute length only once
    if (length > 0)
    {
        direction.SetX(direction.GetX() / length); // Normalize direction
        direction.SetY(direction.GetY() / length);
    }

    float angleRadians = atan2(direction.GetY(), direction.GetX()); // Get rotation in radians
    float angleDegrees = angleRadians * (180.0f / 3.14159265359f); // Convert to degrees if needed

    transform.orientation.SetX(angleDegrees);
    if (angleDegrees < -90 || angleDegrees > 90) 
    {
        transform.scale.SetY(-std::abs(transform.scale.GetY()));
    }
    else 
    {
        transform.scale.SetY(std::abs(transform.scale.GetY()));
    }

    float speed = 0.5f; // 3 for testing; 1.5 for actual
    physics.velocity = direction * speed;
    transform.position.SetX(transform.position.GetX() + physics.velocity.GetX());
    transform.position.SetY(transform.position.GetY() + physics.velocity.GetY());

    // If close enough to waypoint, switch to the next one
    if (squaredDist < threshold)
    {
        if (currentWPIndex == currentWaypoints.size() - 1)
        {
            currentWPIndex = 0; // Loop back to first waypoint
        }
        else
        {
            currentWPIndex++;
        }
    }

}
// ==================================== PATROL STATE IMPLEMENTATION ==================================== //

// ==================================== CHASE STATE IMPLEMENTATION ==================================== //
bool EnemyBehaviour::doesEnemySeePlayer(Entity entity, Entity playerEntity) {
    auto& enemyTransform = ecsCoordinator.getComponent<TransformComponent>(entity);
    auto& enemyComponent = ecsCoordinator.getComponent<EnemyComponent>(entity);
    auto& playerTransform = ecsCoordinator.getComponent<TransformComponent>(playerEntity);

    // Calculate vector to player
    myMath::Vector2D dirToPlayer = playerTransform.position - enemyTransform.position;
    float distanceToPlayer = static_cast<float>(std::sqrt(std::pow(dirToPlayer.GetX(), 2) + std::pow(dirToPlayer.GetY(), 2)));

    // Check if player is within vision distance
    if (distanceToPlayer > enemyComponent.visionDistance) {
        return false;
    }

    // Normalize direction vector
    myMath::Vector2D normalizedDirToPlayer = dirToPlayer;
    float length = static_cast<float>(std::sqrt(std::pow(normalizedDirToPlayer.GetX(), 2) + std::pow(normalizedDirToPlayer.GetY(), 2)));
    if (length > 0) {
        normalizedDirToPlayer.SetX(normalizedDirToPlayer.GetX() / length);
        normalizedDirToPlayer.SetY(normalizedDirToPlayer.GetY() / length);
    }

    // Get the forward vector based on enemy orientation and facing direction
    myMath::Vector2D forwardVector;
    if (isFacingRight) {
        forwardVector.SetX(1.0f);
    }
    else {
        forwardVector.SetX(-1.0f);
    }
    forwardVector.SetY(0.0f);

    // Apply rotation from entity orientation
    float radians = enemyTransform.orientation.GetX() * (3.14159265358979323846 / 180.0);
    float cosTheta = static_cast<float>(std::cos(radians));
    float sinTheta = static_cast<float>(std::sin(radians));
    float rotatedX = forwardVector.GetX() * cosTheta - forwardVector.GetY() * sinTheta;
    float rotatedY = forwardVector.GetX() * sinTheta + forwardVector.GetY() * cosTheta;
    forwardVector.SetX(rotatedX);
    forwardVector.SetY(rotatedY);

    // Calculate dot product between forward vector and direction to player
    float dotProduct = forwardVector.GetX() * normalizedDirToPlayer.GetX() +
        forwardVector.GetY() * normalizedDirToPlayer.GetY();

    // Convert cone half-angle to radians and calculate cosine
    float halfAngleRadians = (enemyComponent.visionAngle / 2.0f) * (3.14159265358979323846 / 180.0);
    float cosHalfAngle = static_cast<float>(std::cos(halfAngleRadians));

    // Check if player is within the cone angle
    if (dotProduct < cosHalfAngle) {
        return false;
    }

    // At this point, player is within vision distance and angle

	if (isWallBlockingVision(enemyTransform.position, playerTransform.position)) {
		return false;
	}

    return true;
}

bool EnemyBehaviour::isWallBlockingVision(myMath::Vector2D enemyPos, myMath::Vector2D playerPos) {
    for (auto entity : ecsCoordinator.getAllLiveEntities()) {
        if (ecsCoordinator.hasComponent<ClosestPlatform>(entity)) {
            auto& wallTransform = ecsCoordinator.getComponent<TransformComponent>(entity);
            myMath::Vector2D wallMin = { wallTransform.position.GetX() - (wallTransform.scale.GetX() / 2),
                                         wallTransform.position.GetY() - (wallTransform.scale.GetY() / 2) };
			myMath::Vector2D wallMax = { wallTransform.position.GetX() + (wallTransform.scale.GetX() / 2),
										 wallTransform.position.GetY() + (wallTransform.scale.GetY() / 2) };

            float tMin = 0.0f;
            float tMax = 0.0f;
            if (rayIntersectAABB(enemyPos, playerPos, wallMin, wallMax, tMin, tMax)) {
				// If player distance is closer to enemy than wall, then wall is not blocking vision
				float playerDist = static_cast<float>(std::sqrt(std::pow(playerPos.GetX() - enemyPos.GetX(), 2.0) + std::pow(playerPos.GetY() - enemyPos.GetY(), 2)));
				float wallDist = static_cast<float>(std::sqrt(std::pow(wallTransform.position.GetX() - enemyPos.GetX(), 2.0) + std::pow(wallTransform.position.GetY() - enemyPos.GetY(), 2)));
				std::cout << "Player dist: " << playerDist << ", " << "Wall dist: " << wallDist << std::endl;


				//it will only return true if wallDist is less than playerDist
                if (wallDist < playerDist) {
                    return true; // Ray is blocked by a wall
                }
            }
        }
    }


	return false;
}

bool EnemyBehaviour::rayIntersectAABB(myMath::Vector2D rayOrigin, myMath::Vector2D rayDirection, myMath::Vector2D aabbMin, myMath::Vector2D aabbMax, float& tMin, float& tMax) {
	tMin = (aabbMin.GetX() - rayOrigin.GetX()) / rayDirection.GetX();
	tMax = (aabbMax.GetX() - rayOrigin.GetX()) / rayDirection.GetX();
	if (tMin > tMax) {
		float temp = tMin;
		tMin = tMax;
		tMax = temp;
	}
	float tyMin = (aabbMin.GetY() - rayOrigin.GetY()) / rayDirection.GetY();
	float tyMax = (aabbMax.GetY() - rayOrigin.GetY()) / rayDirection.GetY();
	if (tyMin > tyMax) {
		float temp = tyMin;
		tyMin = tyMax;
		tyMax = temp;
	}
	if ((tMin > tyMax) || (tyMin > tMax)) {
		return false;
	}
	if (tyMin > tMin) {
		tMin = tyMin;
	}
	if (tyMax < tMax) {
		tMax = tyMax;
	}
	return true;
}


void EnemyBehaviour::updateChaseState(Entity entity) {
    auto PhysicsSystemRef = ecsCoordinator.getSpecificSystem<PhysicsSystemECS>();
    auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
    auto& physics = ecsCoordinator.getComponent<PhysicsComponent>(entity);
    auto& forceManager = ecsCoordinator.getComponent<PhysicsComponent>(entity).forceManager;

    // Get player entity
    auto playerEntity = ecsCoordinator.getEntityFromID("player");
    myMath::Vector2D playerPos = ecsCoordinator.getComponent<TransformComponent>(playerEntity).position;

    // Calculate direction to player
    myMath::Vector2D dirToPlayer = playerPos - transform.position;

    float distanceToPlayer = std::sqrt(std::pow(dirToPlayer.GetX(), 2) + std::pow(dirToPlayer.GetY(), 2));

    // Check if player is within vision distance
    auto& enemyComponent = ecsCoordinator.getComponent<EnemyComponent>(entity);

    if (!doesEnemySeePlayer(entity, playerEntity)) {
        if (distanceToPlayer > enemyComponent.visionDistance) {
            switchState(PATROL);
            return;
        }
    }

    // Normalize the direction vector
    if (distanceToPlayer > 0) {
        dirToPlayer.SetX(dirToPlayer.GetX() / distanceToPlayer);
        dirToPlayer.SetY(dirToPlayer.GetY() / distanceToPlayer);
    }

    // Calculate rotation angle - same as in patrol state
    float angleRadians = atan2(dirToPlayer.GetY(), dirToPlayer.GetX());
    float angleDegrees = angleRadians * (180.0f / 3.14159265359f);

    // Set rotation
    transform.orientation.SetX(angleDegrees);

    // Handle flipping based on angle
    if (angleDegrees < -90 || angleDegrees > 90)
    {
        transform.scale.SetY(-std::abs(transform.scale.GetY()));
    }
    else
    {
        transform.scale.SetY(std::abs(transform.scale.GetY()));
    }

    // Set max speed limit
    const float maxSpeed = 0.5f; // Slightly faster than patrol
    physics.velocity = dirToPlayer * maxSpeed;
    transform.position.SetX(transform.position.GetX() + physics.velocity.GetX());
    transform.position.SetY(transform.position.GetY() + physics.velocity.GetY());

    // Update isFacingRight based on direction
    if (dirToPlayer.GetX() > 0) {
        isFacingRight = true;
    }
    else {
        isFacingRight = false;
    }

}

// ==================================== CHASE STATE IMPLEMENTATION ==================================== //