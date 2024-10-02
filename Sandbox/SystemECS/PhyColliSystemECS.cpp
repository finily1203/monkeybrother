#include "ECSCoordinator.h"
#include "PhyColliSystemECS.h"
#include "TransformComponent.h"
#include "AABBComponent.h"
#include "MovementComponent.h"
#include "ClosestPlatform.h"
#include "GlobalCoordinator.h"
#include "GraphicsSystem.h"
#include <unordered_set>

#define M_PI   3.14159265358979323846264338327950288

PhysicsSystemECS::PhysicsSystemECS() : velocity{ 0, 0 }, gravity{ -20.f }, jumpForce{ 11.f }, friction{ 0.1f }, alrJumped{ false }, collisionPoint{ 0,0 } {}

void PhysicsSystemECS::initialise() {}

void PhysicsSystemECS::cleanup() {}

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

Entity PhysicsSystemECS::FindClosestPlatform(Entity player) {
    float closestDistance = 100000.f;  // Initialize to a large number

    // Find the closest platform to the player
    //for (Platform& currentPlatform : platforms) {
    //    float distance = std::sqrt(
    //        std::pow(player->GetCoordinate().x - currentPlatform.px, 2) +
    //        std::pow(player->GetCoordinate().y - currentPlatform.py, 2)
    //    );

    //    // Keep track of the closest platform
    //    if (distance < closestDistance) {
    //        closestDistance = distance;
    //        closestPlatform = &currentPlatform;
    //    }
    //}
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
                std::pow(playerPos.x - platformPos.x, 2) +
                std::pow(playerPos.y - platformPos.y, 2)
            );

            //std::cout << "Distance for platform " << count << ": " << distance << std::endl;

            if(distance < closestDistance) {
				closestDistance = distance;
                ecsCoordinator.getComponent<ClosestPlatform>(platform).isClosest = true;
                isClosest = count;
                closestPlatform = platform;
			}
        }
    }
    std::cout << "Closest platform: " << isClosest << std::endl;

    return closestPlatform;
}

void PhysicsSystemECS::HandleCollision(Entity closestPlatform, Entity player) {
    if (!closestPlatform) return;

    //CollisionSide collisionSide = collision->circleRectCollision(
    //    player->GetCoordinate().x, player->GetCoordinate().y,
    //    player->GetScale().x, *closestPlatform
    //);

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

                if (GLFWFunctions::isJump) {
                    SetVelocity({ velocityX, jumpForce });
                    GLFWFunctions::isJump = false;
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
        SetCollisionPoint({ 0, 0 });
        //physics->SetVelocity({ velocityX, velocityY });
        ApplyGravity(player, GLFWFunctions::delta_time);
    }
}

void PhysicsSystemECS::HandleSideCollision(int collisionSide, float circleRadius, Entity player, Entity closestPlatform) {
    float playerX = ecsCoordinator.getComponent<TransformComponent>(player).position.x;
    float playerY = ecsCoordinator.getComponent<TransformComponent>(player).position.y;
    float velocityX = GetVelocity().x;

    if (collisionSide == CollisionSide::LEFT) {
        ecsCoordinator.getComponent<TransformComponent>(player).position.x = ecsCoordinator.getComponent<AABBComponent>(closestPlatform).left -circleRadius;
        //player->SetCoordinate({ closestPlatform->left - circleRadius, playerY });
        velocityX = std::max(velocityX, 0.0f);  // Ensure it’s moving right
    }
    else if (collisionSide == CollisionSide::RIGHT) {
        ecsCoordinator.getComponent<TransformComponent>(player).position.x = ecsCoordinator.getComponent<AABBComponent>(closestPlatform).right - circleRadius;
        //player->SetCoordinate({ closestPlatform->right + circleRadius, playerY });
        velocityX = std::min(velocityX, 0.0f);  // Ensure it’s moving left
    }

    velocityX *= -0.5f;  // Adjust bounce effect as needed
    SetVelocity({ velocityX, GetVelocity().y });
}

void PhysicsSystemECS::HandleSlopeCollision(Entity closestPlatform, Entity player) {
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

    float playerScaleXSqrd = ecsCoordinator.getComponent<TransformComponent>(player).scale.x * ecsCoordinator.getComponent<TransformComponent>(player).scale.x;
    // Ensure the player doesn't fall through the slope by correcting Y position
    float opp = abs(GetCollisionPoint().x - playerX);
    float adj = sqrtf(playerScaleXSqrd - opp * opp);
    float platformYAtPlayerX = GetCollisionPoint().y;

    if (playerY - adj < platformYAtPlayerX) {
        //player->SetCoordinate({ playerX, platformYAtPlayerX + adj });
        ecsCoordinator.getComponent<TransformComponent>(player).position.y = platformYAtPlayerX + adj;
        SetVelocity({ velocityX, 0 });
    }

    // Allow jumping from the slope
    if (GLFWFunctions::isJump) {
        SetVelocity({ velocityX, jumpForce });
        GLFWFunctions::isJump = false;
    }
}

// Apply gravity to the player
void PhysicsSystemECS::ApplyGravityAABB() {
    float velocityX = GetVelocity().x;
    float velocityY = GetVelocity().y;
    SetVelocity({ GetVelocity().x, velocityY + gravity * GLFWFunctions::delta_time });
}

void PhysicsSystemECS::HandleAABBCollision(Entity player, Entity closestPlatform) {
    //CollisionSystemECS::AABB playerAABB = { { player->GetCoordinate().x - player->GetScale().x * 0.5f, player->GetCoordinate().y - player->GetScale().y * 0.5f },
    //                            { player->GetCoordinate().x + player->GetScale().x * 0.5f, player->GetCoordinate().y + player->GetScale().y * 0.5f } };

    CollisionSystemECS::AABB playerAABB = {
    { ecsCoordinator.getComponent<TransformComponent>(player).position.x - ecsCoordinator.getComponent<TransformComponent>(player).scale.x * 0.5f,
      ecsCoordinator.getComponent<TransformComponent>(player).position.y - ecsCoordinator.getComponent<TransformComponent>(player).scale.y * 0.5f },
    { ecsCoordinator.getComponent<TransformComponent>(player).position.x + ecsCoordinator.getComponent<TransformComponent>(player).scale.x * 0.5f,
      ecsCoordinator.getComponent<TransformComponent>(player).position.y + ecsCoordinator.getComponent<TransformComponent>(player).scale.y * 0.5f }
    };

    //Collision::AABB platformAABB = { { closestPlatform->left, closestPlatform->bottom },
    //                                { closestPlatform->right, closestPlatform->top } };

    CollisionSystemECS::AABB platformAABB = {
    { ecsCoordinator.getComponent<AABBComponent>(closestPlatform).left, ecsCoordinator.getComponent<AABBComponent>(closestPlatform).bottom },
    { ecsCoordinator.getComponent<AABBComponent>(closestPlatform).right, ecsCoordinator.getComponent<AABBComponent>(closestPlatform).top }
    };

    float firstTimeOfCollision{};

    float slopeAngle = ecsCoordinator.getComponent<TransformComponent>(closestPlatform).orientation.x * (M_PI / 180.f);
    if (slopeAngle == 0) {
        if (collisionSystem.CollisionIntersection_RectRect(platformAABB, { 0, 0 }, playerAABB, GetVelocity(), firstTimeOfCollision)) {
            float overlapX = std::min(playerAABB.max.x - platformAABB.min.x, platformAABB.max.x - playerAABB.min.x);
            float overlapY = std::min(playerAABB.max.y - platformAABB.min.y, platformAABB.max.y - playerAABB.min.y);
            glm::vec2 playerPos = ecsCoordinator.getComponent<TransformComponent>(player).position;
            glm::vec2 playerScl = ecsCoordinator.getComponent<TransformComponent>(player).scale;
            glm::vec2 platformPos = ecsCoordinator.getComponent<TransformComponent>(closestPlatform).position;
            // left, right, top, bottom
            if (overlapX < overlapY) {

                if (playerAABB.max.x > platformAABB.min.x && 
                    playerPos.x < platformPos.x) {
                    //player->SetCoordinate({ platformAABB.min.x - player->GetScale().x * 0.5f, player->GetCoordinate().y });
                    playerPos.x = platformAABB.min.x - playerScl.x * 0.5f;
                    SetVelocity({ 0, GetVelocity().y });
                }
                else if (playerAABB.min.x < platformAABB.max.x &&
                    playerPos.x > platformPos.x) { // Collision from the right
                    //player->SetCoordinate({ platformAABB.max.x + player->GetScale().x * 0.5f, player->GetCoordinate().y });
                    playerPos.x = platformAABB.max.x + playerScl.x * 0.5f;
                    SetVelocity({ 0, GetVelocity().y });
                }
            }
            else {
                if (playerAABB.min.y < platformAABB.max.y &&
                    playerPos.y > platformPos.y) { // Collision from above
                    //player->SetCoordinate({ player->GetCoordinate().x, platformAABB.max.y + player->GetScale().y * 0.5f });
                    playerPos.y = platformAABB.max.y + playerScl.y * 0.5f;
                    SetVelocity({ GetVelocity().x, 0 });
                }
                else if (playerAABB.max.y > platformAABB.min.y &&
                    playerPos.y < platformPos.y) { // Collision from below
                    //player->SetCoordinate({ player->GetCoordinate().x, platformAABB.min.y - player->GetScale().y * 0.5f });
                    playerPos.y = platformAABB.min.y - playerScl.y * 0.5f;
                    SetVelocity({ GetVelocity().x, -GetVelocity().y * GLFWFunctions::delta_time });
                }
            }
        }
        else {
            //No collision detected
            ApplyGravityAABB();
        }
    }
    else {
        if (collisionSystem.AABBSlopeCollision(closestPlatform, player, GetVelocity())) {
            // collision against slanted platform can be detected. Can do some debugging to show there is collision
            //std::cout << "Collision detected" << std::endl;
        }
        else {
            //No collision detected
            //ApplyGravityAABB();
        }
    }
    ApplyGravityAABB();

}

// Smooth horizontal movement when pressing A or D
void PhysicsSystemECS::HandlePlayerInput(Entity player) {
    //float speed = player->GetSpeed() * 1.5f; // faster horizontal movement
    //float maxSpeed = player->GetSpeed() * 2.f; // limit the player's speed
    float speed = ecsCoordinator.getComponent<MovementComponent>(player).speed * 1.5f;
    float maxSpeed = ecsCoordinator.getComponent<MovementComponent>(player).speed * 2.f;

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
        // Apply friction to slow down the player when no key is pressed
        SetVelocity({ GetVelocity().x * 0.9f, GetVelocity().y });  // Adjust friction for smooth deceleration
    }

    // Jump input
    if (GLFWFunctions::isJump) {
        SetVelocity({ GetVelocity().x, jumpForce });
        GLFWFunctions::isJump = false;
    }
}

bool CollisionSystemECS::CollisionIntersection_RectRect(const AABB& aabb1,
                                                        const glm::vec2& vel1,
                                                        const AABB& aabb2,
                                                        const glm::vec2& vel2,
                                                        float& firstTimeOfCollision)
{
    if (aabb1.max.x < aabb2.min.x || aabb1.max.y < aabb2.min.y || aabb1.min.x > aabb2.max.x || aabb1.min.y > aabb2.max.y) { // if no overlap
        //Step 2: Initialize and calculate new velocity of Vb
        float tFirst = 0;
        float tLast = GLFWFunctions::delta_time;

        //AEVec2 Vb;
        //AEVec2Set(&Vb, vel1.x - vel2.x, vel1.y - vel2.y);
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

bool CollisionSystemECS::AABBSlopeCollision(Entity platform, Entity player, glm::vec2 velocity) {
    // Step 1: Define player's AABB
    //CollisionSystemECS::AABB playerAABB = {
    //    { player->GetCoordinate().x - player->GetScale().x * 0.5f,
    //      player->GetCoordinate().y - player->GetScale().y * 0.5f },
    //    { player->GetCoordinate().x + player->GetScale().x * 0.5f, 
    //      player->GetCoordinate().y + player->GetScale().y * 0.5f }
    //};

    CollisionSystemECS::AABB playerAABB = {
        { ecsCoordinator.getComponent<TransformComponent>(player).position.x - ecsCoordinator.getComponent<TransformComponent>(player).scale.x * 0.5f,
          ecsCoordinator.getComponent<TransformComponent>(player).position.y - ecsCoordinator.getComponent<TransformComponent>(player).scale.y * 0.5f },
        { ecsCoordinator.getComponent<TransformComponent>(player).position.x + ecsCoordinator.getComponent<TransformComponent>(player).scale.x * 0.5f,
          ecsCoordinator.getComponent<TransformComponent>(player).position.y + ecsCoordinator.getComponent<TransformComponent>(player).scale.y * 0.5f }
    };

    // Step 2: Define platform's AABB (before rotation)
    //CollisionSystemECS::AABB platformAABB = { { obj.left, obj.bottom }, { obj.right, obj.top } };
    CollisionSystemECS::AABB platformAABB = {
		{ ecsCoordinator.getComponent<AABBComponent>(platform).left, ecsCoordinator.getComponent<AABBComponent>(platform).bottom },
		{ ecsCoordinator.getComponent<AABBComponent>(platform).right, ecsCoordinator.getComponent<AABBComponent>(platform).top }
	};

    // Step 3: Compute the sine and cosine of the platform's rotation angle
    float radians = ecsCoordinator.getComponent<TransformComponent>(platform).orientation.x * (M_PI / 180.f);
    float s = sin(-radians);
    float c = cos(-radians);

    // Step 4: Translate the player's AABB to the platform's local space (relative to platform's origin)
    //AEVec2 translatedMin = { playerAABB.min.x - obj.px, playerAABB.min.y - obj.py };
    //AEVec2 translatedMax = { playerAABB.max.x - obj.px, playerAABB.max.y - obj.py };

    glm::vec2 translatedMin = { playerAABB.min.x - ecsCoordinator.getComponent<TransformComponent>(platform).position.x, 
                                playerAABB.min.y - ecsCoordinator.getComponent<TransformComponent>(platform).position.y };

    glm::vec2 translatedMax = { playerAABB.max.x - ecsCoordinator.getComponent<TransformComponent>(platform).position.x, 
								playerAABB.max.y - ecsCoordinator.getComponent<TransformComponent>(platform).position.y };

    // Step 5: Rotate the translated AABB points (counter-clockwise to match platform rotation)
    glm::vec2 rotatedMin = {
        translatedMin.x * c - translatedMin.y * s,
        translatedMin.x * s + translatedMin.y * c
    };
    glm::vec2 rotatedMax = {
        translatedMax.x * c - translatedMax.y * s,
        translatedMax.x * s + translatedMax.y * c
    };

    // Step 6: Translate the rotated AABB back to world coordinates
    //rotatedMin.x += obj.px;
    //rotatedMin.y += obj.py;
    //rotatedMax.x += obj.px;
    //rotatedMax.y += obj.py;
    rotatedMin.x += ecsCoordinator.getComponent<TransformComponent>(platform).position.x;
    rotatedMin.y += ecsCoordinator.getComponent<TransformComponent>(platform).position.y;
    rotatedMax.x += ecsCoordinator.getComponent<TransformComponent>(platform).position.x;
    rotatedMax.y += ecsCoordinator.getComponent<TransformComponent>(platform).position.y;

    // Create a rotated AABB for the player
    CollisionSystemECS::AABB rotatedPlayerAABB = {
        { rotatedMin.x, rotatedMin.y },
        { rotatedMax.x, rotatedMax.y }
    };

    // Step 7: Check for collision between the rotated player's AABB and the platform's AABB
    float firstTimeOfCollision = 0;
    bool collisionDetected = CollisionIntersection_RectRect(platformAABB, { 0, 0 }, rotatedPlayerAABB, velocity, firstTimeOfCollision);

    return collisionDetected;
}

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

        //PhysicsSystemECS::SetCollisionPoint({ intersectionX, intersectionY });
        //std::cout << "Collision point: " << intersectionX << ", " << intersectionY << std::endl;
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
                return CollisionSide::RIGHTEDGE;
            }
            else if (rotatedX < -halfExtentX) {
                // Collision from the top-left corner
                return CollisionSide::LEFTEDGE;
            }
            else {
                // Collision from the top side
                return CollisionSide::TOP;
            }
        }
        else {
            if (rotatedX > halfExtentX) {
                // Collision from the right side
                return CollisionSide::RIGHT;
            }
            else if (rotatedX < -halfExtentX) {
                // Collision from the left side
                return CollisionSide::LEFT;
            }
            else {
                // Collision from the bottom side
                return CollisionSide::BOTTOM;
            }

        }

    }

    // If no collision was detected, return NONE
    return CollisionSide::NONE;
}

void PhysicsSystemECS::update(float dt) {
    //let it be the first entity
    Entity playerEntity = ecsCoordinator.getFirstEntity();
    Entity closestPlatformEntity = ecsCoordinator.getFirstEntity();

    for (auto& entity : entities) {
        bool isPlayer = ecsCoordinator.hasComponent<MovementComponent>(entity);
        if (isPlayer) {
			playerEntity = entity;
			break;
		}
    }

    closestPlatformEntity = FindClosestPlatform(playerEntity);
    
    //bool isClosestPlatform = ecsCoordinator.getComponent<ClosestPlatform>(closestPlatformEntity).isClosest;
    //std::cout << "Is closes platform really closest? " << (isClosestPlatform ? "yes" : "no") << std::endl;

    HandleAABBCollision(playerEntity, closestPlatformEntity);
    HandlePlayerInput(playerEntity);

    //ecsCoordinator.getComponent<TransformComponent>(playerEntity).position.x += GetVelocity().x;
    //ecsCoordinator.getComponent<TransformComponent>(playerEntity).position.y += GetVelocity().y;
}