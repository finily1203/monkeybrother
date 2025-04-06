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
                               90%
         Jing Wen (jingwen.lee): modified patrol and chase state for EnemyBehaviour
                                 10%
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

}

void EnemyBehaviour::switchState(STATE newState) {
    currentState = newState;
}

void EnemyBehaviour::update(Entity entity) {
    myMath::Vector2D velocity = ecsCoordinator.getComponent<PhysicsComponent>(entity).velocity;

    //update waypoints of entity if it has been changed
    if (hasWaypointsBeenChanged) {
        waypoints = ecsCoordinator.getComponent<EnemyComponent>(entity).waypoints;
        hasWaypointsBeenChanged = false;
    }

    // Check if enemy sees player regardless of avoidance state
    auto playerEntity = ecsCoordinator.getEntityFromID("player");
    bool enemySeePlayer = doesEnemySeePlayer(entity, playerEntity);

    if (enemySeePlayer) {
        if (currentState != ATTACK) {
            isAvoidingWalls = false; // Cancel avoidance if player is spotted

            switchState(CHASE);
            auto& enemy = ecsCoordinator.getComponent<EnemyComponent>(entity);
            enemy.currState = CHASE;
            attackAnimationCreated = false;

            if (!chaseAnimationCreated) {
                createChaseAnimation(entity);
                chaseAnimationCreated = true;
            }
        }
    }

    bool collision = checkPlayerCollision(entity, playerEntity);
    if (collision) {
        if (GLFWFunctions::godMode) return;
        switchState(ATTACK);
        std::cout << "Player Collision Detected" << std::endl;
        chaseAnimationCreated = false;

        if (!attackAnimationCreated) {
            GLFWFunctions::attackAudio = true;
            createAttackAnimation(entity, playerEntity);
            attackAnimationCreated = true;
        }
    }

    switch (currentState) {
    case PATROL:
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

// ==================================== PATROL STATE IMPLEMENTATION ==================================== //

std::vector<myMath::Vector2D>& EnemyBehaviour::getWaypoints() {
    return waypoints;
}

int& EnemyBehaviour::getCurrentWaypointIndex() {
    return currentWaypointIndex;
}

void EnemyBehaviour::updatePatrolState(Entity entity) {
    auto PhysicsSystemRef = ecsCoordinator.getSpecificSystem<PhysicsSystemECS>();
    auto collisionSystem = PhysicsSystemRef->getCollisionSystem();
    auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
    auto& physics = ecsCoordinator.getComponent<PhysicsComponent>(entity);
    auto& currentWaypoints = getWaypoints();
    int& currentWPIndex = getCurrentWaypointIndex();

    if (currentWaypoints.empty()) return;

    Console::GetLog() << "Waypoints: " << currentWaypoints.size() << std::endl;

    myMath::Vector2D selectedDirection;
    bool foundClearPath = false;
    CollisionSystemECS::OBBv2 fishOBB = collisionSystem.createOBBFromEntityv2(entity);

    float closestDistSq = std::numeric_limits<float>::max();
    int closestUnblockedIndex = -1;

    for (int i = 0; i < currentWaypoints.size(); ++i) {
        int tryIndex = (currentWPIndex + i) % currentWaypoints.size();
        myMath::Vector2D potentialTarget = currentWaypoints[tryIndex];
        myMath::Vector2D tryDir = potentialTarget - transform.position;
        float distSq = tryDir.GetX() * tryDir.GetX() + tryDir.GetY() * tryDir.GetY();
        float length = sqrt(distSq);

        if (length > 0) {
            tryDir.SetX(tryDir.GetX() / length);
            tryDir.SetY(tryDir.GetY() / length);
        }

        bool pathBlocked = false;
        fishOBB.center = transform.position + tryDir * 10.0f;

        for (auto wallEntity : ecsCoordinator.getAllLiveEntities()) {
            if (!ecsCoordinator.hasComponent<ClosestPlatform>(wallEntity)) continue;

            auto wallOBB = collisionSystem.createOBBFromEntityv2(wallEntity);
            myMath::Vector2D normal{};
            float penetration{};

            if (collisionSystem.checkOBBOBBCollision(fishOBB, wallOBB, normal, penetration)) {
                pathBlocked = true;
                break;
            }
        }

        if (!pathBlocked) {
            if (distSq < closestDistSq) {
                closestDistSq = distSq;
                closestUnblockedIndex = tryIndex;
                selectedDirection = tryDir;
            }

            if (i == 0) { // Prefer current index if already valid
                currentWPIndex = tryIndex;
                foundClearPath = true;
                break;
            }
        }
    }

    float speed = 0.4f;

    if (foundClearPath) {
        float angleRadians = atan2(selectedDirection.GetY(), selectedDirection.GetX());
        float angleDegrees = angleRadians * (180.0f / 3.14159265359f);
        transform.orientation.SetX(angleDegrees);

        transform.scale.SetY((angleDegrees < -90 || angleDegrees > 90) ?
            -std::abs(transform.scale.GetY()) :
            std::abs(transform.scale.GetY()));

        physics.velocity = selectedDirection * speed;
        transform.position += physics.velocity;

        // If close enough to the current waypoint, go to next
        myMath::Vector2D toTarget = currentWaypoints[currentWPIndex] - transform.position;
        float distSq = toTarget.GetX() * toTarget.GetX() + toTarget.GetY() * toTarget.GetY();
        if (distSq < 100.0f) {
            currentWPIndex = (currentWPIndex + 1) % currentWaypoints.size();
        }
    }
    else {
        Console::GetLog() << "No clear path to any waypoint! Resetting position to closest reachable.\n";
        if (closestUnblockedIndex != -1) {
            transform.position = currentWaypoints[closestUnblockedIndex];
            currentWPIndex = (closestUnblockedIndex + 1) % currentWaypoints.size();
        }
        else {
            Console::GetLog() << "Still completely stuck. No unblocked waypoint.\n";
            physics.velocity = myMath::Vector2D(0, 0); // Fallback
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
    if (distanceToPlayer > 0) {
        normalizedDirToPlayer.SetX(normalizedDirToPlayer.GetX() / distanceToPlayer);
        normalizedDirToPlayer.SetY(normalizedDirToPlayer.GetY() / distanceToPlayer);
    }

    // Get the forward vector directly from the entity's orientation
    float radians = enemyTransform.orientation.GetX() * (3.14159265358979323846f / 180.0f);
    myMath::Vector2D forwardVector(std::cos(radians), std::sin(radians));

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
    myMath::Vector2D dirToPlayer = playerPos - enemyPos;
    float playerDist = static_cast<float>(std::sqrt(std::pow(dirToPlayer.GetX(), 2.0) + std::pow(dirToPlayer.GetY(), 2.0)));

    for (auto entity : ecsCoordinator.getAllLiveEntities()) {
        if (ecsCoordinator.hasComponent<ClosestPlatform>(entity)) {
            auto& wallTransform = ecsCoordinator.getComponent<TransformComponent>(entity);
            myMath::Vector2D wallMin = { wallTransform.position.GetX() - (wallTransform.scale.GetX() / 2),
                                         wallTransform.position.GetY() - (wallTransform.scale.GetY() / 2) };
            myMath::Vector2D wallMax = { wallTransform.position.GetX() + (wallTransform.scale.GetX() / 2),
                                         wallTransform.position.GetY() + (wallTransform.scale.GetY() / 2) };

            float tMin = 0.0f;
            float tMax = 1.0f;  // Important: set max to 1.0 to represent the full ray

            if (rayIntersectAABB(enemyPos, dirToPlayer, wallMin, wallMax, tMin, tMax)) {
                // If tMin is between 0 and 1, there's an intersection along the ray to player
                if (tMin >= 0.0f && tMin <= 1.0f) {
                    // Calculate the actual intersection point
                    myMath::Vector2D intersectionPoint = enemyPos + dirToPlayer * tMin;

                    // Calculate the distance to the intersection point
                    myMath::Vector2D dirToIntersection = intersectionPoint - enemyPos;
                    float intersectionDist = static_cast<float>(std::sqrt(
                        std::pow(dirToIntersection.GetX(), 2.0) +
                        std::pow(dirToIntersection.GetY(), 2.0)));

                    // If the intersection is closer than the player, then the wall is blocking
                    if (intersectionDist < playerDist) {
                        // Optional: Add debug visualization of the intersection point
                        std::cout << "Wall blocking at distance: " << intersectionDist
                            << " (player at " << playerDist << ")" << std::endl;
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool EnemyBehaviour::rayIntersectAABB(myMath::Vector2D rayOrigin, myMath::Vector2D rayDirection,
    myMath::Vector2D aabbMin, myMath::Vector2D aabbMax,
    float& tMin, float& tMax) {
    // Calculate inverse of ray direction to avoid division by zero
    myMath::Vector2D invDir;
    invDir.SetX(rayDirection.GetX() != 0 ? 1.0f / rayDirection.GetX() : FLT_MAX);
    invDir.SetY(rayDirection.GetY() != 0 ? 1.0f / rayDirection.GetY() : FLT_MAX);

    // Calculate t-values for intersections with each plane of the AABB
    float t1x = (aabbMin.GetX() - rayOrigin.GetX()) * invDir.GetX();
    float t2x = (aabbMax.GetX() - rayOrigin.GetX()) * invDir.GetX();
    float t1y = (aabbMin.GetY() - rayOrigin.GetY()) * invDir.GetY();
    float t2y = (aabbMax.GetY() - rayOrigin.GetY()) * invDir.GetY();

    // Find the largest min t-value and smallest max t-value
    tMin = std::max(std::min(t1x, t2x), std::min(t1y, t2y));
    tMax = std::min(std::max(t1x, t2x), std::max(t1y, t2y));

    // If tMax < 0, the ray is intersecting the AABB, but the entire AABB is behind the ray
    // If tMin > tMax, the ray doesn't intersect the AABB
    return tMax >= 0 && tMin <= tMax;
}

bool EnemyBehaviour::AABBIntersect(const myMath::Vector2D& min1, const myMath::Vector2D& max1,
    const myMath::Vector2D& min2, const myMath::Vector2D& max2) {
    return (min1.GetX() < max2.GetX() && max1.GetX() > min2.GetX() &&
        min1.GetY() < max2.GetY() && max1.GetY() > min2.GetY());
}

void EnemyBehaviour::updateChaseState(Entity entity) {
    auto PhysicsSystemRef = ecsCoordinator.getSpecificSystem<PhysicsSystemECS>();
    auto collisionSystem = PhysicsSystemRef->getCollisionSystem();
    auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
    auto& physics = ecsCoordinator.getComponent<PhysicsComponent>(entity);
    auto& enemyComponent = ecsCoordinator.getComponent<EnemyComponent>(entity);

    // Get player position
    auto playerEntity = ecsCoordinator.getEntityFromID("player");
    myMath::Vector2D playerPos = ecsCoordinator.getComponent<TransformComponent>(playerEntity).position;
    float playerRadius = ecsCoordinator.getComponent<TransformComponent>(playerEntity).scale.GetX() * 0.5f;

    // Direction and distance to player
    myMath::Vector2D dir = playerPos - transform.position;
    float distance = std::sqrt(dir.GetX() * dir.GetX() + dir.GetY() * dir.GetY());

    // Vision check
    if (!doesEnemySeePlayer(entity, playerEntity) || distance > enemyComponent.visionDistance) {
        switchState(PATROL);
        enemyComponent.currState = PATROL;
        chaseAnimationCreated = false;
        attackAnimationCreated = false;
        return;
    }
    else {
        chaseAnimationCreated = true;
    }

    // Normalize direction
    if (distance != 0.0f) {
        dir.SetX(dir.GetX() / distance);
        dir.SetY(dir.GetY() / distance);
    }

    // Chase speed
    const float speed = 0.5f;
    myMath::Vector2D desiredMove = dir * speed;
    myMath::Vector2D nextPos = transform.position + desiredMove;

    float radius = transform.scale.GetX() * 0.5f;
    bool collided = false;
    myMath::Vector2D finalMove = desiredMove;

    // Check for collision with walls
    for (auto wallEntity : ecsCoordinator.getAllLiveEntities()) {
        if (!ecsCoordinator.hasComponent<ClosestPlatform>(wallEntity)) continue;

        CollisionSystemECS::OBB wallOBB = collisionSystem.createOBBFromEntity(wallEntity);
        myMath::Vector2D normal{};
        float penetration{};

        if (collisionSystem.checkCircleOBBCollision(nextPos, radius, wallOBB, normal, penetration)) {
            collided = true;

            // Slide along wall
            float dot = myMath::DotProductVector2D(desiredMove, normal);
            finalMove = desiredMove - normal * dot;

            // If the slide movement is too small, stop to avoid jitter
            if (myMath::DotProductVector2D(finalMove, finalMove) < 0.01f) {
                finalMove = { 0.0f, 0.0f };
            }

            break;
        }
    }

    // Check for collision with player
    float distanceToPlayer = static_cast<float>(std::sqrt(std::pow(transform.position.GetX() - playerPos.GetX(), 2) + std::pow(transform.position.GetY() - playerPos.GetY(), 2)));
    if (distanceToPlayer <= radius + playerRadius) {
        switchState(ATTACK);
        std::cout << "Player Collision Detected" << std::endl;
        chaseAnimationCreated = false;

        if (!attackAnimationCreated) {
            GLFWFunctions::attackAudio = true;
            createAttackAnimation(entity, playerEntity);
            attackAnimationCreated = true;
        }
        return;
    }

    // Apply movement
    physics.velocity = finalMove;
    transform.position += finalMove;

    // Face direction
    float angle = atan2(dir.GetY(), dir.GetX()) * (180.f / 3.14159265359f);
    transform.orientation.SetX(angle);
    transform.scale.SetY(angle < -90.f || angle > 90.f ? -std::abs(transform.scale.GetY()) : std::abs(transform.scale.GetY()));
    isFacingRight = (dir.GetX() > 0);
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
    float mouthRadius = enemyTransform.scale.GetX() * 0.5f;

    if (distance < (mouthRadius + playerRadius)) {
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

    ecsCoordinator.setTextureID(entity, "goldfishAlert");
    auto& enemyAnimation = ecsCoordinator.getComponent<AnimationComponent>(entity);
    enemyAnimation.totalFrames = 5;
    enemyAnimation.frameTime = 0.9f;
    enemyAnimation.columns = 2;
    enemyAnimation.rows = 3;
}

void EnemyBehaviour::createAttackAnimation(Entity entity, Entity playerEntity) {
    Entity newAnimationEntity = ecsCoordinator.createEntity();

    ecsCoordinator.setEntityID(newAnimationEntity, "fishAttackAnimation");
    ecsCoordinator.setTextureID(newAnimationEntity, "biteAnimation");

    // Transform setup (Alert should be slightly offset from enemy position)
    TransformComponent transform{};
    auto& playerTransform = ecsCoordinator.getComponent<TransformComponent>(playerEntity);
    transform.position = playerTransform.position;

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