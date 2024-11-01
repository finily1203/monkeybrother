/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Lee Jing Wen (jingwen.lee)
@team: MonkeHood
@course: CSD2401
@file: PhyColliSystemECS.cpp
@brief: This source file contains the implementation of the Physics and Collision System for ECS
        Lee Jing Wen (jingwen.lee): Physics System, Collision System, movement (left, right, jump),
                                    handling collision and integration with ECS.
                                    100%
*//*    _________________________________________________________________-*/
#include "ECSCoordinator.h"
#include "PhyColliSystemECS.h"
#include "TransformComponent.h"
#include "AABBComponent.h"
#include "MovementComponent.h"
#include "ClosestPlatform.h"
#include "GraphicsComponent.h"
#include "GlobalCoordinator.h"
#include "GraphicsSystem.h"
#include <unordered_set>
#include "AudioSystem.h"


#define M_PI   3.14159265358979323846264338327950288f

// PHYSICS SYSTEM

// Constructor for Physics System
PhysicsSystemECS::PhysicsSystemECS() : velocity{ 0, 0 }, gravity{ -0.5f }, jumpForce{ .4f }, friction{ 0.1f }, alrJumped{ false }, isFalling{ false }, eventSource("EventSource"), eventObserver(std::make_shared<Observer>("EventObserver")) 
{
    eventObserver->AttachHandler(MessageId::FALL, [](messagePtr msg) {
        auto fallMsg = std::dynamic_pointer_cast<FallMessage>(msg);
        if (fallMsg)
        {
            std::cout << "Observer: Entity " << fallMsg->GetPlayer() << " is falling" << std::endl;
        }
    });

    eventObserver->AttachHandler(MessageId::JUMP, [](messagePtr msg) {
        auto jumpMsg = std::dynamic_pointer_cast<JumpMessage>(msg);
        if (jumpMsg)
        {
            std::cout << "Observer: Entity " << jumpMsg->GetPlayer() << " is jumping" << std::endl;
        }
    });

    eventSource.Register(MessageId::FALL, eventObserver);
    eventSource.Register(MessageId::JUMP, eventObserver);
}


void PhysicsSystemECS::initialise() {}

void PhysicsSystemECS::cleanup() {}

// Applying gravity to player when no collision detected (jumping, falling)
void PhysicsSystemECS::ApplyGravity(Entity player, float dt)
{
    float velocityX = GetVelocity().x;
    float velocityY = GetVelocity().y;

    // Apply gravity to vertical velocity (no friction for vertical motion)
    velocityY += gravity * dt;

    // Apply friction to horizontal velocity
    velocityX *= (1 - friction * dt);

    ecsCoordinator.getComponent<TransformComponent>(player).position.x += velocityX * dt;
    ecsCoordinator.getComponent<TransformComponent>(player).position.y += velocityY * dt;

    SetVelocity({ velocityX, velocityY });
}

// Find the closest platform to the player
Entity PhysicsSystemECS::FindClosestPlatform(Entity player) {
    float closestDistance = 100000.f;  // Initialize to a large number

    glm::vec2 playerPos = ecsCoordinator.getComponent<TransformComponent>(player).position;
    int count = 0;
    int isClosest = 0;
    Entity closestPlatform = player;
    for (auto& platform : entities) {
        if (platform == player) {
            continue;
        }
        bool hasClosestPlatform = ecsCoordinator.hasComponent<ClosestPlatform>(platform);

        if (hasClosestPlatform) {
            count++;
            glm::vec2 platformPos = ecsCoordinator.getComponent<TransformComponent>(platform).position;
            float distance = std::sqrt(
                std::pow(playerPos.x - platformPos.x, 2.f) +
                std::pow(playerPos.y - platformPos.y, 2.f)
            );

            if (distance < closestDistance) {
                closestDistance = distance;
                ecsCoordinator.getComponent<ClosestPlatform>(platform).isClosest = true;
                isClosest = count;
                closestPlatform = platform;
            }
        }
    }

    return closestPlatform;
}

// Handling slope collision for the player
void PhysicsSystemECS::HandleSlopeCollision(Entity closestPlatform, Entity player) {
    float angleRad = ecsCoordinator.getComponent<TransformComponent>(closestPlatform).orientation.x * (M_PI / 180.f);
    float slopeSin = std::sin(angleRad);
    float slopeCos = std::cos(angleRad);

    // Calculate the gravity force along the slope (parallel to the surface)
    float gravityAlongSlope = gravity * slopeSin;
    float gravityPerpendicularSlope = gravity * slopeCos;

    // Update velocity along the slope (X direction)
    float velocityX = GetVelocity().x + gravityAlongSlope * GLFWFunctions::delta_time;
    float velocityY = GetVelocity().y + gravityPerpendicularSlope * GLFWFunctions::delta_time;

    // Apply friction along the slope to reduce sliding speed gradually
    velocityX *= (1 - friction * GLFWFunctions::delta_time);

    // Update the player's X position based on velocity along the slope
    float playerX = ecsCoordinator.getComponent<TransformComponent>(player).position.x;
    float playerY = ecsCoordinator.getComponent<TransformComponent>(player).position.y;

    ecsCoordinator.getComponent<TransformComponent>(player).position.x = playerX + velocityX * GLFWFunctions::delta_time;

    float halfOfLength = ecsCoordinator.getComponent<TransformComponent>(player).scale.x * 0.5f;
    glm::vec2 collisionPoint = collisionSystem.GetCollisionPoint();
    float expectedPlayerX = collisionPoint.x + halfOfLength + (playerY - collisionPoint.y) * slopeCos;

    // Only adjust Y if the player is below the slope surface
    if (playerX <= expectedPlayerX) {
        // Gently adjust Y position and reset Y velocity
        ecsCoordinator.getComponent<TransformComponent>(player).position.x = expectedPlayerX;
    }
    else {
        // Apply gravity perpendicular to the slope to prevent floating up
        SetVelocity({ velocityX, velocityY });
    }

    if (GLFWFunctions::move_jump_flag) {
        SetVelocity({ velocityX, jumpForce });
    }
}

// Collision detection and handling (physics) for the player
void PhysicsSystemECS::HandleAABBCollision(Entity player, Entity closestPlatform)
{
    CollisionSystemECS::AABB playerAABB = {
    { ecsCoordinator.getComponent<TransformComponent>(player).position.x - ecsCoordinator.getComponent<TransformComponent>(player).scale.x * 0.5f,
      ecsCoordinator.getComponent<TransformComponent>(player).position.y - ecsCoordinator.getComponent<TransformComponent>(player).scale.y * 0.5f },
    { ecsCoordinator.getComponent<TransformComponent>(player).position.x + ecsCoordinator.getComponent<TransformComponent>(player).scale.x * 0.5f,
      ecsCoordinator.getComponent<TransformComponent>(player).position.y + ecsCoordinator.getComponent<TransformComponent>(player).scale.y * 0.5f }
    };

    CollisionSystemECS::AABB platformAABB = {
    { ecsCoordinator.getComponent<AABBComponent>(closestPlatform).left, ecsCoordinator.getComponent<AABBComponent>(closestPlatform).bottom },
    { ecsCoordinator.getComponent<AABBComponent>(closestPlatform).right, ecsCoordinator.getComponent<AABBComponent>(closestPlatform).top }
    };

    float firstTimeOfCollision{};

    float slopeAngle = ecsCoordinator.getComponent<TransformComponent>(closestPlatform).orientation.x * (M_PI / 180.f);
    if (slopeAngle == 0) {
        if (collisionSystem.CollisionIntersection_RectRect(platformAABB, { 0, 0 }, playerAABB, GetVelocity(), firstTimeOfCollision)) {
            if (GLFWFunctions::hasBumped == false) {
                GLFWFunctions::bumpAudio = true;
                GLFWFunctions::hasBumped = true;
            }

            float overlapX = std::min(playerAABB.max.x - platformAABB.min.x, platformAABB.max.x - playerAABB.min.x);
            float overlapY = std::min(playerAABB.max.y - platformAABB.min.y, platformAABB.max.y - playerAABB.min.y);
            glm::vec2 playerPos = ecsCoordinator.getComponent<TransformComponent>(player).position;
            glm::vec2 playerScl = ecsCoordinator.getComponent<TransformComponent>(player).scale;
            glm::vec2 platformPos = ecsCoordinator.getComponent<TransformComponent>(closestPlatform).position;

            // order for checking: left, right, top, bottom
            if (overlapX < overlapY) {

                if (playerAABB.max.x > platformAABB.min.x &&
                    playerPos.x < platformPos.x &&
                    GetVelocity().y != 0 &&
                    GetVelocity().x > 0) {
                    playerPos.x = platformAABB.min.x - playerScl.x * 0.5f;
                    SetVelocity({ 0, GetVelocity().y });
                }
                else if (playerAABB.min.x < platformAABB.max.x &&
                    playerPos.x > platformPos.x &&
                    GetVelocity().y != 0 &&
                    GetVelocity().x < 0) { // Collision from the right
                    playerPos.x = platformAABB.max.x + playerScl.x * 0.5f;

                    SetVelocity({ 0, GetVelocity().y });

                }
            }
            else {
                if (playerAABB.min.y < platformAABB.max.y &&
                    playerPos.y > platformPos.y) { // Collision from above
                    playerPos.y = platformAABB.max.y + playerScl.y * 0.5f;
                    SetVelocity({ GetVelocity().x, 0 });

                    // Allow jumping when on surface of platform
                    if (GLFWFunctions::move_jump_flag) {
                        SetVelocity({ GetVelocity().x, jumpForce });
                    }
                }
                else if (playerAABB.max.y > platformAABB.min.y &&
                    playerPos.y < platformPos.y &&
                    GetVelocity().y > 0 &&
                    GetVelocity().x != 0) { // Collision from below
                    playerPos.y = platformAABB.min.y - playerScl.y * 0.5f;
                    SetVelocity({ GetVelocity().x, -GetVelocity().y * GLFWFunctions::delta_time });
                }

            }
        }
        else {
            SetVelocity({ GetVelocity().x, GetVelocity().y });
            GLFWFunctions::hasBumped = false;
        }
    }
    else {
        if (collisionSystem.AABBSlopeCollision(closestPlatform, player, GetVelocity())) {
            HandleSlopeCollision(closestPlatform, player);

            GLFWFunctions::slideAudio = true;
        }
    }

    ApplyGravity(player, GLFWFunctions::delta_time);
}

// Player input handling for movement (left: 'A', right: 'D')
void PhysicsSystemECS::HandlePlayerInput(Entity player)
{
    float speed = ecsCoordinator.getComponent<MovementComponent>(player).speed;
    float maxSpeed = ecsCoordinator.getComponent<MovementComponent>(player).speed * 8.f;

    // Smooth acceleration for horizontal movement
    float accel = speed * GLFWFunctions::delta_time;  // Adjust for smoothness

    if (GLFWFunctions::move_left_flag) {
        if (GetVelocity().x < -maxSpeed)
            SetVelocity({ GetVelocity().x, GetVelocity().y });
        else
            SetVelocity({ GetVelocity().x - accel, GetVelocity().y });
    }
    else if (GLFWFunctions::move_right_flag) {
        if (GetVelocity().x > maxSpeed)
            SetVelocity({ GetVelocity().x, GetVelocity().y });
        else
            SetVelocity({ GetVelocity().x + accel, GetVelocity().y });
    }
    else {
        if (GetVelocity().x > 0)
            SetVelocity({ GetVelocity().x - accel, GetVelocity().y });  // Adjust friction for smooth deceleration
        else if (GetVelocity().x < 0)
            SetVelocity({ GetVelocity().x + accel, GetVelocity().y });  // Adjust friction for smooth deceleration
    }
}

// PROTOTYPING: Handling Circle vs Rectangle side collision
void PhysicsSystemECS::HandleSideCollision(int collisionSide, float circleRadius, Entity player, Entity closestPlatform) {
    float playerX = ecsCoordinator.getComponent<TransformComponent>(player).position.x;
    float playerY = ecsCoordinator.getComponent<TransformComponent>(player).position.y;
    float velocityX = GetVelocity().x;

    if (collisionSide == CollisionSide::LEFT) {
        ecsCoordinator.getComponent<TransformComponent>(player).position.x = ecsCoordinator.getComponent<AABBComponent>(closestPlatform).left - circleRadius;
        //player->SetCoordinate({ closestPlatform->left - circleRadius, playerY });
        velocityX = std::max(velocityX, 0.0f);  // Ensure its moving right
    }
    else if (collisionSide == CollisionSide::RIGHT) {
        ecsCoordinator.getComponent<TransformComponent>(player).position.x = ecsCoordinator.getComponent<AABBComponent>(closestPlatform).right - circleRadius;
        //player->SetCoordinate({ closestPlatform->right + circleRadius, playerY });
        velocityX = std::min(velocityX, 0.0f);  // Ensure its moving left
    }

    velocityX *= -0.5f;  // Adjust bounce effect as needed
    SetVelocity({ velocityX, GetVelocity().y });
}

// PROTOTYPING: Handling Circle vs Rectangle slope collision (sliding off)
void PhysicsSystemECS::HandleCircleSlopeCollision(Entity closestPlatform, Entity player) {
    float angleRad = ecsCoordinator.getComponent<TransformComponent>(closestPlatform).orientation.x * (M_PI / 180.f);
    float slopeSin = std::sin(angleRad);
    float slopeCos = std::cos(angleRad);

    // Gravity applied along the slope (parallel)
    float gravityAlongSlope = gravity * slopeSin;

    // Update velocity along the slope
    float velocityX = GetVelocity().x + gravityAlongSlope * GLFWFunctions::delta_time;

    // Apply friction to reduce velocity over time, but allow acceleration
    velocityX *= (1 - friction * GLFWFunctions::delta_time);  // Use smaller friction for smoother sliding

    // Update the player's position based on the velocity
    float playerX = ecsCoordinator.getComponent<TransformComponent>(player).position.x;
    float playerY = ecsCoordinator.getComponent<TransformComponent>(player).position.y;

    //player->SetCoordinate({ playerX + velocityX * GLFWFunctions::delta_time, playerY });
    ecsCoordinator.getComponent<TransformComponent>(player).position.x = playerX + velocityX * GLFWFunctions::delta_time;

    // Set velocity in the physics system
    SetVelocity({ velocityX, GetVelocity().y });

    glm::vec2 collisionPoint = collisionSystem.GetCollisionPoint();
    float playerScaleXSqrd = ecsCoordinator.getComponent<TransformComponent>(player).scale.x * ecsCoordinator.getComponent<TransformComponent>(player).scale.x;
    // Ensure the player doesn't fall through the slope by correcting Y position
    float opp = abs(collisionPoint.x - playerX);
    float adj = sqrtf(playerScaleXSqrd - opp * opp);
    float platformYAtPlayerX = collisionPoint.y;

    if (playerY - adj < platformYAtPlayerX) {
        //player->SetCoordinate({ playerX, platformYAtPlayerX + adj });
        ecsCoordinator.getComponent<TransformComponent>(player).position.y = platformYAtPlayerX + adj;
        SetVelocity({ velocityX, 0 });
    }

    if (GLFWFunctions::move_jump_flag) {
        SetVelocity({ velocityX, jumpForce });
    }
}

// PROTYPING: Handling Circle vs Rectangle collision
void PhysicsSystemECS::HandleCircleCollision(Entity closestPlatform, Entity player) {
    if (!closestPlatform) return;

    CollisionSide collisionSide = collisionSystem.circleRectCollision(
        ecsCoordinator.getComponent<TransformComponent>(player).position.x,
        ecsCoordinator.getComponent<TransformComponent>(player).position.y,
        ecsCoordinator.getComponent<TransformComponent>(player).scale.x,
        closestPlatform
    );

    float playerX = ecsCoordinator.getComponent<TransformComponent>(player).position.x;
    float playerY = ecsCoordinator.getComponent<TransformComponent>(player).position.y;
    float velocityX = GetVelocity().x;
    float velocityY = GetVelocity().y;
    float circleRadius = ecsCoordinator.getComponent<TransformComponent>(player).scale.x;

    // Handle collision with the platform
    if (collisionSide != CollisionSide::NONE) {
        float slopeAngle = ecsCoordinator.getComponent<TransformComponent>(closestPlatform).orientation.x * (M_PI / 180.f);
        if (slopeAngle == 0) {
            // Handle flat platform
            if (collisionSide == CollisionSide::TOP) {
                ecsCoordinator.getComponent<TransformComponent>(player).position.y = ecsCoordinator.getComponent<AABBComponent>(closestPlatform).top + circleRadius;
                SetVelocity({ velocityX, 0 });

                if (GLFWFunctions::move_jump_flag) {
                    SetVelocity({ velocityX, jumpForce });
                }
            }
            else if (collisionSide == CollisionSide::BOTTOM) {
                ecsCoordinator.getComponent<TransformComponent>(player).position.y = ecsCoordinator.getComponent<AABBComponent>(closestPlatform).bottom - circleRadius;
                SetVelocity({ velocityX, -velocityY });
            }
            else if (collisionSide == CollisionSide::LEFT || collisionSide == CollisionSide::RIGHT) {
                HandleSideCollision(collisionSide, circleRadius, player, closestPlatform);
            }
        }
        else {
            HandleSlopeCollision(closestPlatform, player);
        }
    }
    else {
        // No collision detected
        collisionSystem.SetCollisionPoint({ 0, 0 });
        ApplyGravity(player, GLFWFunctions::delta_time);
    }
}

// COLLISION SYSTEM

// AABB collision detection
bool CollisionSystemECS::CollisionIntersection_RectRect(const AABB& aabb1,
    const glm::vec2& vel1,
    const AABB& aabb2,
    const glm::vec2& vel2,
    float& firstTimeOfCollision)
{
    if (aabb1.max.x < aabb2.min.x || aabb1.max.y < aabb2.min.y || aabb1.min.x > aabb2.max.x || aabb1.min.y > aabb2.max.y) { // if no overlap

        float tFirst = 0;
        float tLast = GLFWFunctions::delta_time;

        glm::vec2 Vb = vel1 - vel2;

        if (Vb.x < 0) {
            //case 1
            if (aabb1.min.x > aabb2.max.x) {
                return 0;
            }
            //case 4
            if (aabb1.max.x < aabb2.min.x) {
                tFirst = fmax((aabb1.max.x - aabb2.min.x) / Vb.x, tFirst);
            }
            if (aabb1.min.x < aabb2.max.x) {
                tLast = fmin((aabb1.min.x - aabb2.max.x) / Vb.x, tLast);
            }
        }
        else if (Vb.x > 0) {
            // case 2
            if (aabb1.min.x > aabb2.max.x) {
                tFirst = fmax((aabb1.min.x - aabb2.max.x) / Vb.x, tFirst);
            }
            if (aabb1.max.x > aabb2.min.x) {
                tLast = fmin((aabb1.max.x - aabb2.min.x) / Vb.x, tLast);
            }
            //case 3
            if (aabb1.max.x < aabb2.min.x) {
                return 0;
            }
        }
        else {
            // case 5
            if (aabb1.max.x < aabb2.min.x) {
                return 0;
            }
            else if (aabb1.min.x > aabb2.max.x) {
                return 0;
            }
        }

        // case 6
        if (tFirst > tLast) {
            return 0;
        }

        ////////////////////////////////////////////////////////////////////
        if (Vb.y < 0) {
            //case 1
            if (aabb1.min.y > aabb2.max.y) {
                return 0;
            }
            //case 4
            if (aabb1.max.y < aabb2.min.y) {
                tFirst = fmax((aabb1.max.y - aabb2.min.y) / Vb.y, tFirst);
            }
            if (aabb1.min.y < aabb2.max.y) {
                tLast = fmin((aabb1.min.y - aabb2.max.y) / Vb.y, tLast);
            }
        }
        else if (Vb.y > 0) {
            // case 2
            if (aabb1.min.y > aabb2.max.y) {
                tFirst = fmax((aabb1.min.y - aabb2.max.y) / Vb.y, tFirst);
            }
            if (aabb1.max.y > aabb2.min.y) {
                tLast = fmin((aabb1.max.y - aabb2.min.y) / Vb.y, tLast);
            }
            //case 3
            if (aabb1.max.y < aabb2.min.y) {
                return 0;
            }
        }
        else {
            // case 5
            if (aabb1.max.y < aabb2.min.y) {
                return 0;
            }
            else if (aabb1.min.y > aabb2.max.y) {
                return 0;
            }
        }
        // case 6
        if (tFirst > tLast) {
            return 0;
        }
        firstTimeOfCollision = tFirst;

        return 1;
    }
    else {
        return 1;
    }
}

// AABB slope collision detection
bool CollisionSystemECS::AABBSlopeCollision(Entity platform, Entity player, glm::vec2 velocity)
{
    CollisionSystemECS::AABB playerAABB = {
        { ecsCoordinator.getComponent<TransformComponent>(player).position.x - ecsCoordinator.getComponent<TransformComponent>(player).scale.x * 0.5f,
          ecsCoordinator.getComponent<TransformComponent>(player).position.y - ecsCoordinator.getComponent<TransformComponent>(player).scale.y * 0.5f },
        { ecsCoordinator.getComponent<TransformComponent>(player).position.x + ecsCoordinator.getComponent<TransformComponent>(player).scale.x * 0.5f,
          ecsCoordinator.getComponent<TransformComponent>(player).position.y + ecsCoordinator.getComponent<TransformComponent>(player).scale.y * 0.5f }
    };

    // Define platform's AABB (before rotation)
    CollisionSystemECS::AABB platformAABB = {
        { ecsCoordinator.getComponent<AABBComponent>(platform).left, ecsCoordinator.getComponent<AABBComponent>(platform).bottom },
        { ecsCoordinator.getComponent<AABBComponent>(platform).right, ecsCoordinator.getComponent<AABBComponent>(platform).top }
    };

    // Compute the sine and cosine of the platform's rotation angle
    float radians = ecsCoordinator.getComponent<TransformComponent>(platform).orientation.x * (M_PI / 180.f);
    float s = sin(-radians);
    float c = cos(-radians);

    // Translate the player's AABB to the platform's local space (relative to platform's origin)
    glm::vec2 translatedMin = { playerAABB.min.x - ecsCoordinator.getComponent<TransformComponent>(platform).position.x,
                                playerAABB.min.y - ecsCoordinator.getComponent<TransformComponent>(platform).position.y };

    glm::vec2 translatedMax = { playerAABB.max.x - ecsCoordinator.getComponent<TransformComponent>(platform).position.x,
                                playerAABB.max.y - ecsCoordinator.getComponent<TransformComponent>(platform).position.y };

    // Rotate the translated AABB points (counter-clockwise to match platform rotation)
    glm::vec2 rotatedMin = {
        translatedMin.x * c - translatedMin.y * s,
        translatedMin.x * s + translatedMin.y * c
    };
    glm::vec2 rotatedMax = {
        translatedMax.x * c - translatedMax.y * s,
        translatedMax.x * s + translatedMax.y * c
    };

    // Translate the rotated AABB back to world coordinates
    rotatedMin.x += ecsCoordinator.getComponent<TransformComponent>(platform).position.x;
    rotatedMin.y += ecsCoordinator.getComponent<TransformComponent>(platform).position.y;
    rotatedMax.x += ecsCoordinator.getComponent<TransformComponent>(platform).position.x;
    rotatedMax.y += ecsCoordinator.getComponent<TransformComponent>(platform).position.y;

    // Create a rotated AABB for the player
    CollisionSystemECS::AABB rotatedPlayerAABB = {
        { rotatedMin.x, rotatedMin.y },
        { rotatedMax.x, rotatedMax.y }
    };

    // Check for collision between the rotated player's AABB and the platform's AABB
    float firstTimeOfCollision = 0;
    bool collisionDetected = CollisionIntersection_RectRect(platformAABB, { 0, 0 }, rotatedPlayerAABB, velocity, firstTimeOfCollision);

    float angleRad = ecsCoordinator.getComponent<TransformComponent>(platform).orientation.x * (M_PI / 180.f);
    glm::vec2 platformPos = ecsCoordinator.getComponent<TransformComponent>(platform).position;

    // Rotate the circle's center relative to the platform
    float rotatedX = std::cos(angleRad) * (ecsCoordinator.getComponent<TransformComponent>(player).position.x - platformPos.x) + std::sin(angleRad) * (ecsCoordinator.getComponent<TransformComponent>(player).position.y - platformPos.y);
    float rotatedY = -std::sin(angleRad) * (ecsCoordinator.getComponent<TransformComponent>(player).position.x - platformPos.x) + std::cos(angleRad) * (ecsCoordinator.getComponent<TransformComponent>(player).position.y - platformPos.y);

    // Compute half extents of the platform along each axis (for AABB collision)
    glm::vec2 platformScl = ecsCoordinator.getComponent<TransformComponent>(platform).scale;
    float halfExtentX = platformScl.x / 2;
    float halfExtentY = platformScl.y / 2;

    float closestX = std::max(-halfExtentX, std::min(rotatedX, halfExtentX));
    float closestY = std::max(-halfExtentY, std::min(rotatedY, halfExtentY));

    float distanceX = rotatedX - closestX;
    float distanceY = rotatedY - closestY;
    float distanceSquared = distanceX * distanceX + distanceY * distanceY;

    // Compute the distance from the circle center to the rectangle's center along each axis
    float distX = std::abs(rotatedX) - halfExtentX;
    float distY = std::abs(rotatedY) - halfExtentY;

    if (distanceSquared <= 50 * 50) {
        // Intersection detected, transform point back to world coordinates
        glm::vec2 intersectionPoint = {
            platformPos.x + std::cos(-angleRad) * closestX - std::sin(-angleRad) * closestY,
            platformPos.y + std::sin(angleRad) * closestX + std::cos(angleRad) * closestY
        };

        SetCollisionPoint(intersectionPoint);
    }
    return collisionDetected;
}

// PROTOTYPING: Circle vs Rectangle collision detection
CollisionSide CollisionSystemECS::circleRectCollision(float circleX, float circleY, float circleRadius, Entity platform) {
    float angleRad = ecsCoordinator.getComponent<TransformComponent>(platform).orientation.x * (M_PI / 180.f);
    glm::vec2 platformPos = ecsCoordinator.getComponent<TransformComponent>(platform).position;

    // Rotate the circle's center relative to the platform
    float rotatedX = std::cos(angleRad) * (circleX - platformPos.x) + std::sin(angleRad) * (circleY - platformPos.y);
    float rotatedY = -std::sin(angleRad) * (circleX - platformPos.x) + std::cos(angleRad) * (circleY - platformPos.y);

    // Compute half extents of the platform along each axis (for AABB collision)
    glm::vec2 platformScl = ecsCoordinator.getComponent<TransformComponent>(platform).scale;
    float halfExtentX = platformScl.x / 2;
    float halfExtentY = platformScl.y / 2;

    float closestX = std::max(-halfExtentX, std::min(rotatedX, halfExtentX));
    float closestY = std::max(-halfExtentY, std::min(rotatedY, halfExtentY));

    float distanceX = rotatedX - closestX;
    float distanceY = rotatedY - closestY;
    float distanceSquared = distanceX * distanceX + distanceY * distanceY;

    // Compute the distance from the circle center to the rectangle's center along each axis
    float distX = std::abs(rotatedX) - halfExtentX;
    float distY = std::abs(rotatedY) - halfExtentY;

    if (distanceSquared <= circleRadius * circleRadius) {
        // Intersection detected, transform point back to world coordinates
        float intersectionX = platformPos.x + std::cos(-angleRad) * closestX - std::sin(-angleRad) * closestY;
        float intersectionY = platformPos.y + std::sin(angleRad) * closestX + std::cos(angleRad) * closestY;

        SetCollisionPoint({ intersectionX, intersectionY });
    }

    // If the distance is negative, the circle's center is within the rectangle along that axis
    distX = (distX < 0) ? 0 : distX;
    distY = (distY < 0) ? 0 : distY;

    // Check for collision
    if (distX * distX + distY * distY <= circleRadius * circleRadius) {
        // Determine collision side based on distance and circle position
        if (rotatedY > halfExtentY) {
            if (rotatedX > halfExtentX) {
                // Collision from the top-right corner
                std::cout << "TR" << std::endl;
                return CollisionSide::RIGHTEDGE;
            }
            else if (rotatedX < -halfExtentX) {
                // Collision from the top-left corner
                std::cout << "TL" << std::endl;
                return CollisionSide::LEFTEDGE;
            }
            else {
                // Collision from the top side
                std::cout << "T" << std::endl;
                return CollisionSide::TOP;
            }
        }
        else {
            if (rotatedX > halfExtentX) {
                // Collision from the right side
                std::cout << "R" << std::endl;
                return CollisionSide::RIGHT;
            }
            else if (rotatedX < -halfExtentX) {
                // Collision from the left side
                std::cout << "L" << std::endl;
                return CollisionSide::LEFT;
            }
            else {
                // Collision from the bottom side
                std::cout << "B" << std::endl;
                return CollisionSide::BOTTOM;
            }

        }

    }

    // If no collision was detected, return NONE
    return CollisionSide::NONE;
}

// Update function for Physics System
void PhysicsSystemECS::update(float dt) {
    //let it be the first entity
    Entity playerEntity = ecsCoordinator.getFirstEntity();
    Entity closestPlatformEntity = ecsCoordinator.getFirstEntity();   
    //Entity playerEntity;
    //Entity closestPlatformEntity;

    for (auto& entity : entities) {
        bool isPlayer = ecsCoordinator.hasComponent<MovementComponent>(entity);
        if (isPlayer) {
            playerEntity = entity;
            break;
        }
    }

    if (GetVelocity().y < 0 && !isFalling)
    {
        isFalling = true;
        auto fallMsg = std::make_shared<FallMessage>(playerEntity);
        eventSource.ProcessMessage(fallMsg);
    }

    if (GetVelocity().y >= 0 && isFalling)
    {
        isFalling = false;
    }

    if (GetVelocity().y > 0 && !alrJumped)
    {
        alrJumped = true;
        auto jumpMsg = std::make_shared<JumpMessage>(playerEntity);
        eventSource.ProcessMessage(jumpMsg);
    }

    if (GetVelocity().y <= 0 && alrJumped)
    {
        alrJumped = false;
    }

    closestPlatformEntity = FindClosestPlatform(playerEntity);

    HandleAABBCollision(playerEntity, closestPlatformEntity);
    HandlePlayerInput(playerEntity);

    ecsCoordinator.getComponent<TransformComponent>(playerEntity).position.x += GetVelocity().x;
    ecsCoordinator.getComponent<TransformComponent>(playerEntity).position.y += GetVelocity().y;

}

std::string PhysicsSystemECS::getSystemECS() {
    return "PhysicsColliSystemECS";
}