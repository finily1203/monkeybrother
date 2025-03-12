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
    hasWaypointsBeenChanged = true;
    isAvoidingWalls = false;
    avoidTimer = 0.f;
    chaseAnimationCreated = false;
    attackAnimationCreated = false;
    timesAvoided = 0;

	//For now all enemies have same way point
    
    //Test CW
    //waypoints.push_back(myMath::Vector2D(-200, 50));
    ////waypoints.push_back(myMath::Vector2D(200, 200));
    //waypoints.push_back(myMath::Vector2D(300, 100));
    //waypoints.push_back(myMath::Vector2D(-300, -100));

    //waypoints.push_back(myMath::Vector2D(-100, 0));
    //waypoints.push_back(myMath::Vector2D(-200, -200));
    //waypoints.push_back(myMath::Vector2D(-200, -100));
    //waypoints.push_back(myMath::Vector2D(-100, -100));
    //waypoints.push_back(myMath::Vector2D(-100, 100));
    //waypoints.push_back(myMath::Vector2D(-200, 100));

}

void EnemyBehaviour::switchState(STATE newState) {  
   currentState = newState;  
}

void EnemyBehaviour::update(Entity entity) {
    auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
	myMath::Vector2D velocity = ecsCoordinator.getComponent<PhysicsComponent>(entity).velocity;

    //update waypoints of entity if it has been changed
	if (hasWaypointsBeenChanged) {
		waypoints = ecsCoordinator.getComponent<EnemyComponent>(entity).waypoints;
		hasWaypointsBeenChanged = false;
	}

    // Handle avoidance timer if currently avoiding
    if (isAvoidingWalls) {
        avoidTimer -= GLFWFunctions::delta_time; 

        if (avoidTimer <= 0.0f) {
            isAvoidingWalls = false;
            avoidTimer = 0.0f;
            std::cout << "Avoidance complete" << std::endl;

            // If in patrol mode, move to next waypoint
            if (currentState == PATROL && !waypoints.empty()) {
                if (currentWaypointIndex == waypoints.size() - 1) {
                    currentWaypointIndex = 0;
                }
                else {
                    currentWaypointIndex++;
                }
            }
        }
        else {
            // Continue avoiding - move the entity based on its current velocity
            transform.position.SetX(transform.position.GetX() + velocity.GetX());
            transform.position.SetY(transform.position.GetY() + velocity.GetY());

            std::cout << "Still avoiding - Timer: " << avoidTimer << std::endl;
            return; // Skip normal behavior while avoiding
        }
    }

    //Check if enemy is avoiding walls
    if (avoidWalls(entity))
    {
        startAvoid(entity);
        return;
    }

    auto playerEntity = ecsCoordinator.getEntityFromID("player");
	bool enemySeePlayer = doesEnemySeePlayer(entity, playerEntity);
	if (enemySeePlayer) {
        if (currentState != ATTACK) {
            switchState(CHASE);
            auto& enemy = ecsCoordinator.getComponent<EnemyComponent>(entity);
            enemy.currState = CHASE;
            attackAnimationCreated = false;

            if (!chaseAnimationCreated)
            {
				createChaseAnimation(entity);
				chaseAnimationCreated = true;
                
            }
        }
	}

    // Check for player collision if in chase state
    if (currentState == CHASE) {
        bool collision = checkPlayerCollision(entity, playerEntity);
        if (collision) {
            switchState(ATTACK);
			std::cout << "Player Collision Detected" << std::endl;
			chaseAnimationCreated = false;

            if (!attackAnimationCreated)
            {
                createAttackAnimation(entity);
                attackAnimationCreated = true;
            }
        }
    }

	switch (currentState) {
	case PATROL:
		//std::cout << "moving to waypoint " << currentWaypointIndex << std::endl;
		updatePatrolState(entity);
		break;
	case CHASE:
        updateChaseState(entity);
		break;
	case ATTACK:
		updateAttackState(entity);
		break;
	}
}

bool EnemyBehaviour::avoidWalls(Entity entity) {
    auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
    //auto& physics = ecsCoordinator.getComponent<PhysicsComponent>(entity);
	auto& enemy = ecsCoordinator.getComponent<EnemyComponent>(entity);
    auto& currentWaypoints = getWaypoints();
    int& currentWPIndex = getCurrentWaypointIndex();

    // Parameters
    const float minWallDistance = (enemy.visionDistance / 3.f); // Minimum distance to detect walls

    // If no waypoints, just return
    if (currentWaypoints.empty()) {
        return false;
    }

    // Get current target waypoint
    myMath::Vector2D targetWP = currentWaypoints[currentWPIndex];

    // Calculate direction to waypoint (same as in updatePatrolState)
    myMath::Vector2D direction = targetWP - transform.position;

    // Normalize direction
    float length = sqrt(direction.GetX() * direction.GetX() + direction.GetY() * direction.GetY());
    if (length > 0) {
        direction.SetX(direction.GetX() / length);
        direction.SetY(direction.GetY() / length);
    }
    else {
        return false; // If not moving, no need to check for walls
    }

    // Calculate ray end point (a point in the direction of next waypoint)
    myMath::Vector2D rayEnd = transform.position + (direction * minWallDistance);

    // Check all potential wall entities
    for (auto wallEntity : ecsCoordinator.getAllLiveEntities()) {
        if (ecsCoordinator.hasComponent<ClosestPlatform>(wallEntity)) {
            auto& wallTransform = ecsCoordinator.getComponent<TransformComponent>(wallEntity);

            // Create AABB for the wall
            myMath::Vector2D wallMin = {
                wallTransform.position.GetX() - (wallTransform.scale.GetX() / 2),
                wallTransform.position.GetY() - (wallTransform.scale.GetY() / 2)
            };
            myMath::Vector2D wallMax = {
                wallTransform.position.GetX() + (wallTransform.scale.GetX() / 2),
                wallTransform.position.GetY() + (wallTransform.scale.GetY() / 2)
            };

            float tMin = 0.0f;
            float tMax = 1.0f;

            // Check if ray intersects with the wall's AABB
            if (rayIntersectAABB(transform.position, rayEnd, wallMin, wallMax, tMin, tMax)) {
                // Only consider intersections within our minimum distance
                if (tMin >= 0.0f && tMin <= 1.0f) {
                    // Print debug message
                    std::cout << "Wall Detected" << std::endl;

                    return true;
                }
            }
        }
    }

    return false;
}

void EnemyBehaviour::startAvoid(Entity entity) {
    // Only start if not already avoiding
    if (!isAvoidingWalls) {
        auto& physics = ecsCoordinator.getComponent<PhysicsComponent>(entity);
        auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
		auto& enemy = ecsCoordinator.getComponent<EnemyComponent>(entity);

        enemy.currState = PATROL;

        // Store current direction to reverse it
        myMath::Vector2D currentDir = physics.velocity;
        float length = sqrt(currentDir.GetX() * currentDir.GetX() + currentDir.GetY() * currentDir.GetY());

        if (length > 0) {
            // Normalize and reverse direction
            myMath::Vector2D oppositeDir(-currentDir.GetX() / length, -currentDir.GetY() / length);
            float speed = 0.5f; // Same speed as normal movement

            // Set avoiding flag and timer
            isAvoidingWalls = true;
            avoidTimer = 2.0f; // Set to 2 seconds for now

            // Update velocity to move in opposite direction
            physics.velocity = oppositeDir * speed;

            // Update orientation to face the new direction
            float angleRadians = atan2(oppositeDir.GetY(), oppositeDir.GetX());
            float angleDegrees = angleRadians * (180.0f / 3.14159265359f);
            transform.orientation.SetX(angleDegrees);

            // Handle sprite flipping
            if (angleDegrees < -90 || angleDegrees > 90) {
                transform.scale.SetY(-std::abs(transform.scale.GetY()));
            }
            else {
                transform.scale.SetY(std::abs(transform.scale.GetY()));
            }

            // Update facing direction
            if (oppositeDir.GetX() > 0) {
                isFacingRight = true;
            }
            else {
                isFacingRight = false;
            }

            std::cout << "Wall Detected - Starting Avoidance" << std::endl;
        }
    }
}


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
    //auto& forceManager = ecsCoordinator.getComponent<PhysicsComponent>(entity).forceManager;
    myMath::Vector2D gravity = ecsCoordinator.getComponent<PhysicsComponent>(entity).gravityScale;
    //float mass = ecsCoordinator.getComponent<PhysicsComponent>(entity).mass;

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

    float speed = 0.5f; 
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
    float radians = enemyTransform.orientation.GetX() * (3.14159265358979323846f / 180.0f);
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
    float halfAngleRadians = (enemyComponent.visionAngle / 2.0f) * (3.14159265358979323846f / 180.0f);
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
				//std::cout << "Player dist: " << playerDist << ", " << "Wall dist: " << wallDist << std::endl;


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
    //auto& forceManager = ecsCoordinator.getComponent<PhysicsComponent>(entity).forceManager;

    // Get player entity
    auto playerEntity = ecsCoordinator.getEntityFromID("player");
    myMath::Vector2D playerPos = ecsCoordinator.getComponent<TransformComponent>(playerEntity).position;

    // Calculate direction to player
    myMath::Vector2D dirToPlayer = playerPos - transform.position;

    float distanceToPlayer = std::sqrt(std::pow(dirToPlayer.GetX(), 2.f) + std::pow(dirToPlayer.GetY(), 2.f));

    // Check if player is within vision distance
    auto& enemyComponent = ecsCoordinator.getComponent<EnemyComponent>(entity);

    if (!doesEnemySeePlayer(entity, playerEntity)) {
        if (distanceToPlayer > enemyComponent.visionDistance) {
            switchState(PATROL);
			auto& enemy = ecsCoordinator.getComponent<EnemyComponent>(entity);
			enemy.currState = PATROL;
            chaseAnimationCreated = false;
            attackAnimationCreated = false;
            return;
        }
    }

    // Normalize the direction vector
    if (distanceToPlayer > 0) {
        dirToPlayer.SetX(dirToPlayer.GetX() / distanceToPlayer);
        dirToPlayer.SetY(dirToPlayer.GetY() / distanceToPlayer);
    }

    // Calculate rotation angle
    float angleRadians = atan2(dirToPlayer.GetY(), dirToPlayer.GetX());
    float angleDegrees = angleRadians * (180.0f / 3.14159265359f);

    // Set orientation of x based on angle
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

    const float maxSpeed = 0.5f;
    physics.velocity = dirToPlayer * maxSpeed;
    transform.position.SetX(transform.position.GetX() + physics.velocity.GetX());
    transform.position.SetY(transform.position.GetY() + physics.velocity.GetY());

    if (dirToPlayer.GetX() > 0) {
        isFacingRight = true;
    }
    else {
        isFacingRight = false;
    }

}

bool EnemyBehaviour::checkPlayerCollision(Entity enemyEntity, Entity playerEntity) {
    auto& enemyTransform = ecsCoordinator.getComponent<TransformComponent>(enemyEntity);
    auto& playerTransform = ecsCoordinator.getComponent<TransformComponent>(playerEntity);

    // Get the orientation of the enemy (fish)
    float enemyAngle = enemyTransform.orientation.GetX() * (3.14159265359f / 180.0f); // Convert to radians

    // Define the player's collision box (assumed to be circular/spherical)
    float playerRadius = playerTransform.scale.GetX() * 0.5f; // Assuming scale represents diameter

    // Calculate the enemy's facing direction based on orientation
    myMath::Vector2D facingDir(cos(enemyAngle), sin(enemyAngle));

    // Calculate the front area of the enemy where the "mouth" would be
    // (where collision with player should be detected)
    myMath::Vector2D enemyCenter = enemyTransform.position;
    myMath::Vector2D mouthOffset = facingDir * (enemyTransform.scale.GetX() * 0.5f); // Offset to front of fish
    myMath::Vector2D mouthPos = enemyCenter + mouthOffset;

    // Calculate distance from player to the mouth position
    myMath::Vector2D playerToMouth = mouthPos - playerTransform.position;
    float distance = sqrt(playerToMouth.GetX() * playerToMouth.GetX() +
        playerToMouth.GetY() * playerToMouth.GetY());

    // Check if the player is within the mouth area (using a smaller hit area)
    float mouthRadius = enemyTransform.scale.GetX() * 0.3f; // Smaller than the actual width

    if (distance < (mouthRadius + playerRadius)) {
        // Perform a more precise OBB check
        // Transform player position to enemy's local space
        myMath::Vector2D localPlayerPos = playerTransform.position - enemyCenter;

        // Rotate the player position to align with the enemy's orientation
        float cosA = cos(-enemyAngle);
        float sinA = sin(-enemyAngle);
        float rotatedX = localPlayerPos.GetX() * cosA - localPlayerPos.GetY() * sinA;
        float rotatedY = localPlayerPos.GetX() * sinA + localPlayerPos.GetY() * cosA;

        // Now check if the rotated player position is within the enemy's bounding box
        // But primarily focused on the front part (positive x-axis in local space)
        if (rotatedX > 0 && // Only check front half of the fish
            rotatedX < enemyTransform.scale.GetX() * 0.6f && // Front portion
            abs(rotatedY) < enemyTransform.scale.GetY() * 0.5f) {

            return true; // Collision detected in front of fish
        }
    }

    return false; // No collision
}

// ==================================== CHASE STATE IMPLEMENTATION ==================================== //

// ==================================== ATTACK STATE IMPLEMENTATION ==================================== //
void EnemyBehaviour::updateAttackState(Entity entity) {
	auto& enemy = ecsCoordinator.getComponent<EnemyComponent>(entity);
	enemy.currState = ATTACK;
    GLFWFunctions::isPlayerDead = true;
}

// ==================================== ATTACK STATE IMPLEMENTATION ==================================== //


// ==================================== ANIMATION HANDLING ==================================== //

void EnemyBehaviour::createChaseAnimation(Entity entity) {
	Entity newAnimationEntity = ecsCoordinator.createEntity();

	ecsCoordinator.setEntityID(newAnimationEntity, "fishAlertAnimation");
	ecsCoordinator.setTextureID(newAnimationEntity, "alertAnimation");

	// Transform setup (Alert should be slightly offset from enemy position)
 	TransformComponent transform{};
	auto& entityTransform = ecsCoordinator.getComponent<TransformComponent>(entity);

    transform.position = { entityTransform.position.GetX() + 50.0f, entityTransform.position.GetY() + 50.f };
	transform.scale.SetX(100.0f);
	transform.scale.SetY(100.0f);

	ecsCoordinator.addComponent(newAnimationEntity, transform);

	// Animation setup
	AnimationComponent animation{};
	animation.isAnimated = true;
	animation.totalFrames = 12.0f;
	animation.frameTime = 0.1f;
	animation.columns = 4.0f;
	animation.rows = 3.0f;

	ecsCoordinator.addComponent(newAnimationEntity, animation);

	// take layer of entity and add animation to that layer
	int newLayer = layerManager.getEntityLayer(entity);
	layerManager.addEntityToLayer(newLayer, newAnimationEntity);
}

void EnemyBehaviour::createAttackAnimation(Entity entity) {
    Entity newAnimationEntity = ecsCoordinator.createEntity();

    ecsCoordinator.setEntityID(newAnimationEntity, "fishAttackAnimation");
    ecsCoordinator.setTextureID(newAnimationEntity, "biteAnimation");

    // Transform setup (Alert should be slightly offset from enemy position)
    TransformComponent transform{};
    auto& entityTransform = ecsCoordinator.getComponent<TransformComponent>(entity);

    transform.position = { entityTransform.position.GetX() + (entityTransform.scale.GetX() * 0.4f), 
                           entityTransform.position.GetY() - (entityTransform.scale.GetY() * 0.4f) };
    transform.scale.SetX(100.0f);
    transform.scale.SetY(100.0f);

    ecsCoordinator.addComponent(newAnimationEntity, transform);

    // Animation setup
    AnimationComponent animation{};
    animation.isAnimated = true;
    animation.totalFrames = 8.0f;
    animation.frameTime = 0.1f;
    animation.columns = 8.0f;
    animation.rows = 2.0f;

    ecsCoordinator.addComponent(newAnimationEntity, animation);

    // take layer of entity and add animation to that layer
    int newLayer = layerManager.getEntityLayer(entity);
    layerManager.addEntityToLayer(newLayer, newAnimationEntity);
}

// ==================================== ANIMATION HANDLING ==================================== //