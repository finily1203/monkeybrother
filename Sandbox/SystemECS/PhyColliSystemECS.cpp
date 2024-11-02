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
#include "RigidBodyComponent.h"

#include "GlobalCoordinator.h"
#include "GraphicsSystem.h"
#include <unordered_set>
#include "AudioSystem.h"

#define M_PI   3.14159265358979323846264338327950288f

// PHYSICS SYSTEM

// Constructor for Physics System
PhysicsSystemECS::PhysicsSystemECS() : friction{ 0.1f }, alrJumped{ false }, isFalling{ false }, eventSource("PlayerEventSource"), eventObserver(std::make_shared<PlayerActionListener>()) 
{
    eventSource.Register(MessageId::FALL, eventObserver);
    eventSource.Register(MessageId::JUMP, eventObserver);
}


void PhysicsSystemECS::initialise() {}

void PhysicsSystemECS::cleanup() {}

// Applying gravity to player when no collision detected (jumping, falling)
void PhysicsSystemECS::ApplyGravity(Entity player, float dt)
{
    myMath::Vector2D& playerPos = ecsCoordinator.getComponent<TransformComponent>(player).position;

    myMath::Vector2D& vel = ecsCoordinator.getComponent<RigidBodyComponent>(player).velocity;
    float grav = ecsCoordinator.getComponent<RigidBodyComponent>(player).gravityScale;

    // Apply gravity to vertical velocity (no friction for vertical motion)
    vel.SetY(vel.GetY() + grav * dt);
    
    // Apply friction to horizontal velocity
    vel.SetX(vel.GetX() * (1 - friction * dt));

    playerPos.SetX(playerPos.GetX() + (vel.GetX() * dt));
    playerPos.SetY(playerPos.GetY() + (vel.GetY() * dt));


    //float velocityX = GetVelocity().GetX();
    //float velocityY = GetVelocity().GetY();

    //velocityY += gravity * dt;

    // Apply friction to horizontal velocity
    //velocityX *= (1 - friction * dt);

    //ecsCoordinator.getComponent<TransformComponent>(player).position.SetX(ecsCoordinator.getComponent<TransformComponent>(player).position.GetX() + velocityX * dt);
    //ecsCoordinator.getComponent<TransformComponent>(player).position.SetY(ecsCoordinator.getComponent<TransformComponent>(player).position.GetY() + velocityY * dt);


    //SetVelocity({ velocityX, velocityY });
}

// Find the closest platform to the player
Entity PhysicsSystemECS::FindClosestPlatform(Entity player) {
    float closestDistance = 100000.f;  // Initialize to a large number

    //glm::vec2 playerPos = ecsCoordinator.getComponent<TransformComponent>(player).position;
    myMath::Vector2D playerPos = ecsCoordinator.getComponent<TransformComponent>(player).position;
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
            //glm::vec2 platformPos = ecsCoordinator.getComponent<TransformComponent>(platform).position;
            myMath::Vector2D platformPos = ecsCoordinator.getComponent<TransformComponent>(platform).position;
            //float distance = std::sqrt(
            //    std::pow(playerPos.x - platformPos.x, 2.f) +
            //    std::pow(playerPos.y - platformPos.y, 2.f)
            //);            
            float distance = std::sqrt(
                std::pow(playerPos.GetX() - platformPos.GetX(), 2.f) +
                std::pow(playerPos.GetY() - platformPos.GetY(), 2.f)
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
//void PhysicsSystemECS::HandleSlopeCollision(Entity closestPlatform, Entity player) {
//    //float angleRad = ecsCoordinator.getComponent<TransformComponent>(closestPlatform).orientation.x * (M_PI / 180.f);
//    float angleRad = ecsCoordinator.getComponent<TransformComponent>(closestPlatform).orientation.GetX() * (M_PI / 180.f);
//    float slopeSin = std::sin(angleRad);
//    float slopeCos = std::cos(angleRad);
//
//    // Calculate the gravity force along the slope (parallel to the surface)
//    float gravityAlongSlope = gravity * slopeSin;
//    float gravityPerpendicularSlope = gravity * slopeCos;
//
//    // Update velocity along the slope (X direction)
//    //float velocityX = GetVelocity().x + gravityAlongSlope * GLFWFunctions::delta_time;
//    //float velocityY = GetVelocity().y + gravityPerpendicularSlope * GLFWFunctions::delta_time;    
//    float velocityX = GetVelocity().GetX() + gravityAlongSlope * GLFWFunctions::delta_time;
//    float velocityY = GetVelocity().GetY() + gravityPerpendicularSlope * GLFWFunctions::delta_time;
//
//    // Apply friction along the slope to reduce sliding speed gradually
//    velocityX *= (1 - friction * GLFWFunctions::delta_time);
//
//    // Update the player's X position based on velocity along the slope
//    //float playerX = ecsCoordinator.getComponent<TransformComponent>(player).position.x;
//    //float playerY = ecsCoordinator.getComponent<TransformComponent>(player).position.y;
//
//    float playerX = ecsCoordinator.getComponent<TransformComponent>(player).position.GetX();
//    float playerY = ecsCoordinator.getComponent<TransformComponent>(player).position.GetY();
//
//    //ecsCoordinator.getComponent<TransformComponent>(player).position.x = playerX + velocityX * GLFWFunctions::delta_time;
//    ecsCoordinator.getComponent<TransformComponent>(player).position.SetX(playerX + velocityX * GLFWFunctions::delta_time);
//
//    //float halfOfLength = ecsCoordinator.getComponent<TransformComponent>(player).scale.x * 0.5f;
//    float halfOfLength = ecsCoordinator.getComponent<TransformComponent>(player).scale.GetX() * 0.5f;
//    //glm::vec2 collisionPoint = collisionSystem.GetCollisionPoint();
//    myMath::Vector2D collisionPoint = collisionSystem.GetCollisionPoint();
//    //float expectedPlayerX = collisionPoint.x + halfOfLength + (playerY - collisionPoint.y) * slopeCos;
//    float expectedPlayerX = collisionPoint.GetX() + halfOfLength + (playerY - collisionPoint.GetY()) * slopeCos;
//
//    // Only adjust Y if the player is below the slope surface
//    if (playerX <= expectedPlayerX) {
//        // Gently adjust Y position and reset Y velocity
//        //ecsCoordinator.getComponent<TransformComponent>(player).position.x = expectedPlayerX;
//        ecsCoordinator.getComponent<TransformComponent>(player).position.SetX(expectedPlayerX);
//    }
//    else {
//        // Apply gravity perpendicular to the slope to prevent floating up
//        SetVelocity({ velocityX, velocityY });
//    }
//
//    if (GLFWFunctions::move_jump_flag) {
//        SetVelocity({ velocityX, jumpForce });
//    }
//}

// Collision detection and handling (physics) for the player
//void PhysicsSystemECS::HandleAABBCollision(Entity player, Entity closestPlatform)
//{
//    //CollisionSystemECS::AABB playerAABB = {
//    //{ ecsCoordinator.getComponent<TransformComponent>(player).position.x - ecsCoordinator.getComponent<TransformComponent>(player).scale.x * 0.5f,
//    //  ecsCoordinator.getComponent<TransformComponent>(player).position.y - ecsCoordinator.getComponent<TransformComponent>(player).scale.y * 0.5f },
//    //{ ecsCoordinator.getComponent<TransformComponent>(player).position.x + ecsCoordinator.getComponent<TransformComponent>(player).scale.x * 0.5f,
//    //  ecsCoordinator.getComponent<TransformComponent>(player).position.y + ecsCoordinator.getComponent<TransformComponent>(player).scale.y * 0.5f }
//    //};
//    CollisionSystemECS::AABB playerAABB = {
//    { ecsCoordinator.getComponent<TransformComponent>(player).position.GetX() - ecsCoordinator.getComponent<TransformComponent>(player).scale.GetX() * 0.5f,
//      ecsCoordinator.getComponent<TransformComponent>(player).position.GetY() - ecsCoordinator.getComponent<TransformComponent>(player).scale.GetY() * 0.5f},
//    { ecsCoordinator.getComponent<TransformComponent>(player).position.GetX() + ecsCoordinator.getComponent<TransformComponent>(player).scale.GetX() * 0.5f,
//      ecsCoordinator.getComponent<TransformComponent>(player).position.GetY() + ecsCoordinator.getComponent<TransformComponent>(player).scale.GetY() * 0.5f }
//    };
//
//    CollisionSystemECS::AABB platformAABB = {
//    { ecsCoordinator.getComponent<AABBComponent>(closestPlatform).left, ecsCoordinator.getComponent<AABBComponent>(closestPlatform).bottom },
//    { ecsCoordinator.getComponent<AABBComponent>(closestPlatform).right, ecsCoordinator.getComponent<AABBComponent>(closestPlatform).top }
//    };
//
//    float firstTimeOfCollision{};
//
//    //float slopeAngle = ecsCoordinator.getComponent<TransformComponent>(closestPlatform).orientation.x * (M_PI / 180.f);
//    float slopeAngle = ecsCoordinator.getComponent<TransformComponent>(closestPlatform).orientation.GetX() * (M_PI / 180.f);
//    if (slopeAngle == 0) {
//        if (collisionSystem.CollisionIntersection_RectRect(platformAABB, { 0, 0 }, playerAABB, GetVelocity(), firstTimeOfCollision)) {
//            if (GLFWFunctions::hasBumped == false) {
//                GLFWFunctions::bumpAudio = true;
//                GLFWFunctions::hasBumped = true;
//            }
//
//            //float overlapX = std::min(playerAABB.max.x - platformAABB.min.x, platformAABB.max.x - playerAABB.min.x);
//            //float overlapY = std::min(playerAABB.max.y - platformAABB.min.y, platformAABB.max.y - playerAABB.min.y);            
//            float overlapX = std::min(playerAABB.max.GetX() - platformAABB.min.GetX(), platformAABB.max.GetX() - playerAABB.min.GetX());
//            float overlapY = std::min(playerAABB.max.GetY() - platformAABB.min.GetY(), platformAABB.max.GetY() - playerAABB.min.GetY());
//            myMath::Vector2D playerPos = ecsCoordinator.getComponent<TransformComponent>(player).position;
//            myMath::Vector2D playerScl = ecsCoordinator.getComponent<TransformComponent>(player).scale;
//            myMath::Vector2D platformPos = ecsCoordinator.getComponent<TransformComponent>(closestPlatform).position;
//
//            // order for checking: left, right, top, bottom
//            if (overlapX < overlapY) {
//
//                //if (playerAABB.max.x > platformAABB.min.x &&
//                //    playerPos.x < platformPos.x &&
//                //    GetVelocity().y != 0 &&
//                //    GetVelocity().x > 0) {
//                //    playerPos.x = platformAABB.min.x - playerScl.x * 0.5f;
//                //    SetVelocity({ 0, GetVelocity().y });
//                //}            
//                if (playerAABB.max.GetX() > platformAABB.min.GetX() &&
//                    playerPos.GetX() < platformPos.GetX() &&
//                    GetVelocity().GetY() != 0 &&
//                    GetVelocity().GetX() > 0) {
//                    playerPos.SetX(platformAABB.min.GetX() - playerScl.GetX() * 0.5f);
//                    SetVelocity({ 0, GetVelocity().GetY() });
//                }
//                //else if (playerAABB.min.x < platformAABB.max.x &&
//                //    playerPos.x > platformPos.x &&
//                //    GetVelocity().y != 0 &&
//                //    GetVelocity().x < 0) { // Collision from the right
//                //    playerPos.x = platformAABB.max.x + playerScl.x * 0.5f;
//                //
//                //    SetVelocity({ 0, GetVelocity().y });
//                //
//                //}
//                else if (playerAABB.min.GetX() < platformAABB.max.GetX() &&
//                    playerPos.GetX() > platformPos.GetX() &&
//                    GetVelocity().GetY() != 0 &&
//                    GetVelocity().GetX() < 0) { // Collision from the right
//                    playerPos.SetX(platformAABB.max.GetX() + playerScl.GetX() * 0.5f);
//                
//                    SetVelocity({ 0, GetVelocity().GetY() });
//                
//                }
//            }
//            else {
//                //if (playerAABB.min.y < platformAABB.max.y &&
//                //    playerPos.y > platformPos.y) { // Collision from above
//                //    playerPos.y = platformAABB.max.y + playerScl.y * 0.5f;
//                //    SetVelocity({ GetVelocity().x, 0 });
//
//                //    // Allow jumping when on surface of platform
//                //    if (GLFWFunctions::move_jump_flag) {
//                //        SetVelocity({ GetVelocity().x, jumpForce });
//                //    }
//                //}
//                if (playerAABB.min.GetY() < platformAABB.max.GetY() &&
//                    playerPos.GetY() > platformPos.GetY()) { // Collision from above
//                    playerPos.SetY(platformAABB.max.GetY() + playerScl.GetY() * 0.5f);
//                    SetVelocity({ GetVelocity().GetX(), 0});
//
//                    // Allow jumping when on surface of platform
//                    if (GLFWFunctions::move_jump_flag) {
//                        SetVelocity({ GetVelocity().GetX(), jumpForce});
//                    }
//                }
//                //else if (playerAABB.max.y > platformAABB.min.y &&
//                //    playerPos.y < platformPos.y &&
//                //    GetVelocity().y > 0 &&
//                //    GetVelocity().x != 0) { // Collision from below
//                //    playerPos.y = platformAABB.min.y - playerScl.y * 0.5f;
//                //    SetVelocity({ GetVelocity().x, -GetVelocity().y * GLFWFunctions::delta_time });
//                //}                
//                else if (playerAABB.max.GetY() > platformAABB.min.GetY() &&
//                    playerPos.GetY() < platformPos.GetY() &&
//                    GetVelocity().GetY() > 0 &&
//                    GetVelocity().GetX() != 0) { // Collision from below
//                    playerPos.SetY(platformAABB.min.GetY() - playerScl.GetY() * 0.5f);
//                    SetVelocity({ GetVelocity().GetX(), -GetVelocity().GetY() * GLFWFunctions::delta_time});
//                }
//
//            }
//        }
//        else {
//            //SetVelocity({ GetVelocity().x, GetVelocity().y });
//            SetVelocity({ GetVelocity().GetX(), GetVelocity().GetY()});
//            GLFWFunctions::hasBumped = false;
//        }
//    }
//    else {
//        if (collisionSystem.AABBSlopeCollision(closestPlatform, player, GetVelocity())) {
//            HandleSlopeCollision(closestPlatform, player);
//
//            GLFWFunctions::slideAudio = true;
//        }
//    }
//
//    ApplyGravity(player, GLFWFunctions::delta_time);
//}

void PhysicsSystemECS::ApplyForce(Entity player, const myMath::Vector2D& appliedForce) {
    myMath::Vector2D& force = ecsCoordinator.getComponent<RigidBodyComponent>(player).force;
    
    force.SetX(force.GetX() + appliedForce.GetX());
    force.SetY(force.GetY() + appliedForce.GetY());

}

// Player input handling for movement (left: 'A', right: 'D')
void PhysicsSystemECS::HandlePlayerInput(Entity player)
{
 //   //float speed = ecsCoordinator.getComponent<MovementComponent>(player).speed;
 //   //float maxSpeed = ecsCoordinator.getComponent<RigidBodyComponent>(player).velocity.GetX() * 8.f;
 //   const float maxSpeed = 0.6f;

 //   myMath::Vector2D& playerPos = ecsCoordinator.getComponent<TransformComponent>(player).position;
 //   myMath::Vector2D acceleration = ecsCoordinator.getComponent<RigidBodyComponent>(player).acceleration;
 //   myMath::Vector2D& force = ecsCoordinator.getComponent<RigidBodyComponent>(player).force;
 //   float mass = ecsCoordinator.getComponent<RigidBodyComponent>(player).mass;
 //   myMath::Vector2D& vel = ecsCoordinator.getComponent<RigidBodyComponent>(player).velocity;
 //   float gravityScale = ecsCoordinator.getComponent<RigidBodyComponent>(player).gravityScale;

 //   if (GLFWFunctions::move_left_flag) {
 //       ApplyForce(player, myMath::Vector2D(-0.05f, 0.f));
 //   }
 //   else if (GLFWFunctions::move_right_flag) {
 //       ApplyForce(player, myMath::Vector2D(0.05f, 0.f));
 //   }
 //   else {

 //       // Apply friction to gradually slow down
 //       if (force.GetX() > 0) {
 //           ApplyForce(player, myMath::Vector2D(-0.05f, 0.f));
 //       }
 //       else if (force.GetX() < 0) {
 //           ApplyForce(player, myMath::Vector2D(0.05f, 0.f));
 //       }

 //       if (std::abs(force.GetX()) < 0.01f) { // threshold
 //           vel.SetX(0.f);
 //           force.SetX(0.f);
 //       }

 //   }

 //   float invMass = mass > 0.f ? 1.f / mass : 0.f;
 //   acceleration = force * invMass;

 //   // Clamp speed without interfering with velocity
 //   if (vel.GetX() > maxSpeed) {
 //       acceleration.SetX(0);
 //   }
 //   else if (vel.GetX() < -maxSpeed) {
	//	acceleration.SetX(0);
	//}

 //   std::cout << force.GetX() << std::endl;
 //   vel.SetX(vel.GetX() + acceleration.GetX() * GLFWFunctions::delta_time);
 //   vel.SetY(vel.GetY() + acceleration.GetY() * GLFWFunctions::delta_time);
 //   
 //   // Dampening
 //   vel.SetX(vel.GetX() * 0.9f);

 //   playerPos.SetX(playerPos.GetX() + vel.GetX() * GLFWFunctions::delta_time);
 //   playerPos.SetY(playerPos.GetY() + vel.GetY() * GLFWFunctions::delta_time);
}

//// PROTOTYPING: Handling Circle vs Rectangle side collision
//void PhysicsSystemECS::HandleSideCollision(int collisionSide, float circleRadius, Entity player, Entity closestPlatform) {
//    float playerX = ecsCoordinator.getComponent<TransformComponent>(player).position.x;
//    float playerY = ecsCoordinator.getComponent<TransformComponent>(player).position.y;
//    float velocityX = GetVelocity().x;
//
//    if (collisionSide == CollisionSide::LEFT) {
//        ecsCoordinator.getComponent<TransformComponent>(player).position.x = ecsCoordinator.getComponent<AABBComponent>(closestPlatform).left - circleRadius;
//        //player->SetCoordinate({ closestPlatform->left - circleRadius, playerY });
//        velocityX = std::max(velocityX, 0.0f);  // Ensure its moving right
//    }
//    else if (collisionSide == CollisionSide::RIGHT) {
//        ecsCoordinator.getComponent<TransformComponent>(player).position.x = ecsCoordinator.getComponent<AABBComponent>(closestPlatform).right - circleRadius;
//        //player->SetCoordinate({ closestPlatform->right + circleRadius, playerY });
//        velocityX = std::min(velocityX, 0.0f);  // Ensure its moving left
//    }
//
//    velocityX *= -0.5f;  // Adjust bounce effect as needed
//    SetVelocity({ velocityX, GetVelocity().y });
//}
//
//// PROTOTYPING: Handling Circle vs Rectangle slope collision (sliding off)
//void PhysicsSystemECS::HandleCircleSlopeCollision(Entity closestPlatform, Entity player) {
//    float angleRad = ecsCoordinator.getComponent<TransformComponent>(closestPlatform).orientation.x * (M_PI / 180.f);
//    float slopeSin = std::sin(angleRad);
//    float slopeCos = std::cos(angleRad);
//
//    // Gravity applied along the slope (parallel)
//    float gravityAlongSlope = gravity * slopeSin;
//
//    // Update velocity along the slope
//    float velocityX = GetVelocity().x + gravityAlongSlope * GLFWFunctions::delta_time;
//
//    // Apply friction to reduce velocity over time, but allow acceleration
//    velocityX *= (1 - friction * GLFWFunctions::delta_time);  // Use smaller friction for smoother sliding
//
//    // Update the player's position based on the velocity
//    float playerX = ecsCoordinator.getComponent<TransformComponent>(player).position.x;
//    float playerY = ecsCoordinator.getComponent<TransformComponent>(player).position.y;
//
//    //player->SetCoordinate({ playerX + velocityX * GLFWFunctions::delta_time, playerY });
//    ecsCoordinator.getComponent<TransformComponent>(player).position.x = playerX + velocityX * GLFWFunctions::delta_time;
//
//    // Set velocity in the physics system
//    SetVelocity({ velocityX, GetVelocity().y });
//
//    glm::vec2 collisionPoint = collisionSystem.GetCollisionPoint();
//    float playerScaleXSqrd = ecsCoordinator.getComponent<TransformComponent>(player).scale.x * ecsCoordinator.getComponent<TransformComponent>(player).scale.x;
//    // Ensure the player doesn't fall through the slope by correcting Y position
//    float opp = abs(collisionPoint.x - playerX);
//    float adj = sqrtf(playerScaleXSqrd - opp * opp);
//    float platformYAtPlayerX = collisionPoint.y;
//
//    if (playerY - adj < platformYAtPlayerX) {
//        //player->SetCoordinate({ playerX, platformYAtPlayerX + adj });
//        ecsCoordinator.getComponent<TransformComponent>(player).position.y = platformYAtPlayerX + adj;
//        SetVelocity({ velocityX, 0 });
//    }
//
//    if (GLFWFunctions::move_jump_flag) {
//        SetVelocity({ velocityX, jumpForce });
//    }
//}
//
//// PROTYPING: Handling Circle vs Rectangle collision
//void PhysicsSystemECS::HandleCircleCollision(Entity closestPlatform, Entity player) {
//    if (!closestPlatform) return;
//
//    CollisionSide collisionSide = collisionSystem.circleRectCollision(
//        ecsCoordinator.getComponent<TransformComponent>(player).position.x,
//        ecsCoordinator.getComponent<TransformComponent>(player).position.y,
//        ecsCoordinator.getComponent<TransformComponent>(player).scale.x,
//        closestPlatform
//    );
//
//    float playerX = ecsCoordinator.getComponent<TransformComponent>(player).position.x;
//    float playerY = ecsCoordinator.getComponent<TransformComponent>(player).position.y;
//    float velocityX = GetVelocity().x;
//    float velocityY = GetVelocity().y;
//    float circleRadius = ecsCoordinator.getComponent<TransformComponent>(player).scale.x;
//
//    // Handle collision with the platform
//    if (collisionSide != CollisionSide::NONE) {
//        float slopeAngle = ecsCoordinator.getComponent<TransformComponent>(closestPlatform).orientation.x * (M_PI / 180.f);
//        if (slopeAngle == 0) {
//            // Handle flat platform
//            if (collisionSide == CollisionSide::TOP) {
//                ecsCoordinator.getComponent<TransformComponent>(player).position.y = ecsCoordinator.getComponent<AABBComponent>(closestPlatform).top + circleRadius;
//                SetVelocity({ velocityX, 0 });
//
//                if (GLFWFunctions::move_jump_flag) {
//                    SetVelocity({ velocityX, jumpForce });
//                }
//            }
//            else if (collisionSide == CollisionSide::BOTTOM) {
//                ecsCoordinator.getComponent<TransformComponent>(player).position.y = ecsCoordinator.getComponent<AABBComponent>(closestPlatform).bottom - circleRadius;
//                SetVelocity({ velocityX, -velocityY });
//            }
//            else if (collisionSide == CollisionSide::LEFT || collisionSide == CollisionSide::RIGHT) {
//                HandleSideCollision(collisionSide, circleRadius, player, closestPlatform);
//            }
//        }
//        else {
//            HandleSlopeCollision(closestPlatform, player);
//        }
//    }
//    else {
//        // No collision detected
//        collisionSystem.SetCollisionPoint({ 0, 0 });
//        ApplyGravity(player, GLFWFunctions::delta_time);
//    }
//}

// COLLISION SYSTEM

// OBB collision detection
// SAT for OBB vs Circle
CollisionSystemECS::OBB CollisionSystemECS::createOBBFromEntity(Entity entity) {
    OBB obb{};
    auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);

    obb.center = transform.position;
    obb.halfExtents = transform.scale * 0.5f;
    obb.rotation = transform.orientation.GetX() * (M_PI / 180.0f);

    // Calculate local axes
    obb.axes[0] = myMath::Vector2D(cos(obb.rotation), sin(obb.rotation));
    obb.axes[1] = myMath::Vector2D(-sin(obb.rotation), cos(obb.rotation));

    return obb;
}

// Project point onto axis
float CollisionSystemECS::projectPoint(const myMath::Vector2D& point, const myMath::Vector2D& axis) {
    return myMath::DotProductVector2D(point, axis);
}

// Get projection interval of OBB onto axis
void CollisionSystemECS::projectOBB(const OBB& obb, const myMath::Vector2D& axis, float& min, float& max) {
    myMath::Vector2D vertices[4];
    getOBBVertices(obb, vertices);

    min = max = projectPoint(vertices[0], axis);
    for (int i = 1; i < 4; i++) {
        float projection = projectPoint(vertices[i], axis);
        min = std::min(min, projection);
        max = std::max(max, projection);
    }
}

// Get OBB vertices
void CollisionSystemECS::getOBBVertices(const OBB& obb, myMath::Vector2D vertices[4]) {
    vertices[0] = obb.center + obb.axes[0] * obb.halfExtents.GetX() + obb.axes[1] * obb.halfExtents.GetY();
    vertices[1] = obb.center - obb.axes[0] * obb.halfExtents.GetX() + obb.axes[1] * obb.halfExtents.GetY();
    vertices[2] = obb.center - obb.axes[0] * obb.halfExtents.GetX() - obb.axes[1] * obb.halfExtents.GetY();
    vertices[3] = obb.center + obb.axes[0] * obb.halfExtents.GetX() - obb.axes[1] * obb.halfExtents.GetY();
}

// Circle vs OBB collision detection using SAT
bool CollisionSystemECS::checkCircleOBBCollision(const myMath::Vector2D& circleCenter, float radius, const OBB& obb, myMath::Vector2D& normal, float& penetration) {
    // Transform circle center to OBB space
    myMath::Vector2D localCenter = circleCenter - obb.center;
    myMath::Vector2D closest = localCenter;

    // Clamp circle center to OBB bounds
    closest.SetX(std::max(-obb.halfExtents.GetX(), std::min(myMath::DotProductVector2D(localCenter, obb.axes[0]), obb.halfExtents.GetX())));
    closest.SetY(std::max(-obb.halfExtents.GetY(), std::min(myMath::DotProductVector2D(localCenter, obb.axes[1]), obb.halfExtents.GetY())));

    // Transform back to world space
    closest = obb.center + closest.GetX() * obb.axes[0] + closest.GetY() * obb.axes[1];

    // Check if circle intersects with closest point
    myMath::Vector2D diff = circleCenter - closest;
    float distSqr = myMath::DotProductVector2D(diff, diff);

    if (distSqr <= radius * radius) {
        float dist = sqrt(distSqr);
        normal = dist > 0 ? diff / dist : myMath::Vector2D(0, 1);

        penetration = radius - dist;
        return true;
    }

    return false;
}

// OBB vs OBB collision detection using SAT (CAN DETECT OBB COLLISION)
bool CollisionSystemECS::checkOBBCollisionSAT(const OBB& obb1, const OBB& obb2, myMath::Vector2D& normal, float& penetration) {
    // Test axes from both OBBs
    const myMath::Vector2D* axes[] = {
        &obb1.axes[0], &obb1.axes[1],
        &obb2.axes[0], &obb2.axes[1]
    };

    //penetration = std::numeric_limits<float>::infinity();

    for (int i = 0; i < 4; i++) {
        float min1, max1, min2, max2;
        projectOBB(obb1, *axes[i], min1, max1);
        projectOBB(obb2, *axes[i], min2, max2);

        // Check for separation
        if (min1 > max2 || min2 > max1) {
            return false;
        }

        //// Have to fix this part
        //float axisDepth = std::min(max2 - min1, max1 - min2);
        //if (axisDepth < penetration) {
        //    penetration = axisDepth;
        //    normal = *axes[i];

        //    // Ensure normal points from obb1 to obb2
        //    myMath::Vector2D centerDiff = obb2.center - obb1.center;
        //    if (glm::dot(centerDiff, normal) < 0) {
        //        normal = -normal;
        //    }
        //}
    }
    return true;
}

void PhysicsSystemECS::HandleCircleOBBCollision(Entity player, Entity platform) {
    myMath::Vector2D& playerPos = ecsCoordinator.getComponent<TransformComponent>(player).position;
    myMath::Vector2D plat = ecsCoordinator.getComponent<TransformComponent>(platform).position;
    myMath::Vector2D& vel = ecsCoordinator.getComponent<RigidBodyComponent>(player).velocity;
    float radius = ecsCoordinator.getComponent<TransformComponent>(player).scale.GetX() * 0.5f;
    myMath::Vector2D directionVector;
    myMath::NormalizeVector2D(directionVector, vel);
    CollisionSystemECS::OBB playerOBB = collisionSystem.createOBBFromEntity(player);
    CollisionSystemECS::OBB platformOBB = collisionSystem.createOBBFromEntity(platform);

    myMath::Vector2D normal{};
    float penetration{};

    if (collisionSystem.checkCircleOBBCollision(playerPos, radius, platformOBB, normal, penetration)) {
        //printf("playerPos: %f, %f \n", playerPos.x, playerPos.y);
		//printf("platform: %f, %f \n", plat.x, plat.y);
        playerPos += normal * penetration;  // Adjust position to resolve penetration
        //std::cout << directionVector.GetX() << ", " << directionVector.GetY() << std::endl;
        // change this so that when top of player is in collision instead of checking bottom of platform in collision (direction of player)
        //// Check if collision is from below
        //bool isBottomCollision = (normal.y < 0);

        //if (isBottomCollision) {
        //    // Invert the y-component of velocity on bottom collision to simulate bounce
        //    SetVelocity({ GetVelocity().x, -GetVelocity().y });
        //}

        if (normal.GetX() != 0) {
            ApplyGravity(player, GLFWFunctions::delta_time);
        }


        //printf("velocity: %f, %f\n", GetVelocity().x, GetVelocity().y);
        // Allow jumping
        if (GLFWFunctions::move_jump_flag) {
            myMath::Vector2D& force = ecsCoordinator.getComponent<RigidBodyComponent>(player).force;
            float jumping = ecsCoordinator.getComponent<RigidBodyComponent>(player).jump;
            
            vel.SetY(jumping);
        }
    }
    else {
        // No collision, apply gravity
        ApplyGravity(player, GLFWFunctions::delta_time);
    }

    //if (collisionSystem.checkOBBCollisionSAT(playerOBB, platformOBB, normal, penetration)) {
    //    // Collision occurred! Handle the response
    //    auto& transform1 = ecsCoordinator.getComponent<TransformComponent>(player);

    //    // Move entity1 out of collision
    //    transform1.position += normal * (penetration);
    //}
    //else {
    //    // No collision, apply gravity
    //    ApplyGravity(player, GLFWFunctions::delta_time);
    //}
}

// AABB collision detection
bool CollisionSystemECS::CollisionIntersection_RectRect(const AABB& aabb1,
    const myMath::Vector2D& vel1,
    const AABB& aabb2,
    const myMath::Vector2D& vel2,
    float& firstTimeOfCollision)
{
    //if (aabb1.max.x < aabb2.min.x || aabb1.max.y < aabb2.min.y || aabb1.min.x > aabb2.max.x || aabb1.min.y > aabb2.max.y) { // if no overlap
    if (aabb1.max.GetX() < aabb2.min.GetX() || aabb1.max.GetY() < aabb2.min.GetY() || aabb1.min.GetX() > aabb2.max.GetX() || aabb1.min.GetY() > aabb2.max.GetY()) { // if no overlap

        float tFirst = 0;
        float tLast = GLFWFunctions::delta_time;

        //glm::vec2 Vb = vel1 - vel2;
        myMath::Vector2D Vb = vel1 - vel2;

        //if (Vb.x < 0) {
        if (Vb.GetX() < 0) {
            //case 1
            //if (aabb1.min.x > aabb2.max.x) {
            if (aabb1.min.GetX() > aabb2.max.GetX()) {
                return 0;
            }
            //case 4
            //if (aabb1.max.x < aabb2.min.x) {
            if (aabb1.max.GetX() < aabb2.min.GetX()) {
                //tFirst = fmax((aabb1.max.x - aabb2.min.x) / Vb.x, tFirst);
                tFirst = fmax((aabb1.max.GetX() - aabb2.min.GetX()) / Vb.GetX(), tFirst);
            }
            //if (aabb1.min.x < aabb2.max.x) {
            if (aabb1.min.GetX() < aabb2.max.GetX()) {
                //tLast = fmin((aabb1.min.x - aabb2.max.x) / Vb.x, tLast);
                tLast = fmin((aabb1.min.GetX() - aabb2.max.GetX()) / Vb.GetX(), tLast);
            }
        }
        //else if (Vb.x > 0) {
        else if (Vb.GetX() > 0) {
            // case 2
            //if (aabb1.min.x > aabb2.max.x) {
            if (aabb1.min.GetX() > aabb2.max.GetX()) {
                //tFirst = fmax((aabb1.min.x - aabb2.max.x) / Vb.x, tFirst);
                tFirst = fmax((aabb1.min.GetX() - aabb2.max.GetX()) / Vb.GetX(), tFirst);
            }
            //if (aabb1.max.x > aabb2.min.x) {
            if (aabb1.max.GetX() > aabb2.min.GetX()) {
                tLast = fmin((aabb1.max.GetX() - aabb2.min.GetX()) / Vb.GetX(), tLast);
            }
            //case 3
            //if (aabb1.max.x < aabb2.min.x) {
            if (aabb1.max.GetX() < aabb2.min.GetX()) {
                return 0;
            }
        }
        else {
            // case 5
            //if (aabb1.max.x < aabb2.min.x) {
            if (aabb1.max.GetX() < aabb2.min.GetX()) {
                return 0;
            }
            //else if (aabb1.min.x > aabb2.max.x) {
            else if (aabb1.min.GetX() > aabb2.max.GetX()) {
                return 0;
            }
        }

        // case 6
        if (tFirst > tLast) {
            return 0;
        }

        ////////////////////////////////////////////////////////////////////
        //if (Vb.y < 0) {
        if (Vb.GetY() < 0) {
            //case 1
            //if (aabb1.min.y > aabb2.max.y) {
            if (aabb1.min.GetY() > aabb2.max.GetY()) {
                return 0;
            }
            //case 4
            //if (aabb1.max.y < aabb2.min.y) {
            if (aabb1.max.GetY() < aabb2.min.GetY()) {
                //tFirst = fmax((aabb1.max.y - aabb2.min.y) / Vb.y, tFirst);
                tFirst = fmax((aabb1.max.GetY() - aabb2.min.GetY()) / Vb.GetY(), tFirst);
            }
            //if (aabb1.min.y < aabb2.max.y) {
            if (aabb1.min.GetY() < aabb2.max.GetY()) {
                //tLast = fmin((aabb1.min.y - aabb2.max.y) / Vb.y, tLast);
                tLast = fmin((aabb1.min.GetY() - aabb2.max.GetY()) / Vb.GetY(), tLast);
            }
        }
        //else if (Vb.y > 0) {
        else if (Vb.GetY() > 0) {
            // case 2
            //if (aabb1.min.y > aabb2.max.y) {
            if (aabb1.min.GetY() > aabb2.max.GetY()) {
                //tFirst = fmax((aabb1.min.y - aabb2.max.y) / Vb.y, tFirst);
                tFirst = fmax((aabb1.min.GetY() - aabb2.max.GetY()) / Vb.GetY(), tFirst);
            }
            //if (aabb1.max.y > aabb2.min.y) {
            if (aabb1.max.GetY() > aabb2.min.GetY()) {
                //tLast = fmin((aabb1.max.y - aabb2.min.y) / Vb.y, tLast);
                tLast = fmin((aabb1.max.GetY() - aabb2.min.GetY()) / Vb.GetY(), tLast);
            }
            //case 3
            //if (aabb1.max.y < aabb2.min.y) {
            if (aabb1.max.GetY() < aabb2.min.GetY()) {
                return 0;
            }
        }
        else {
            // case 5
            //if (aabb1.max.y < aabb2.min.y) {
            if (aabb1.max.GetY() < aabb2.min.GetY()) {
                return 0;
            }
            //else if (aabb1.min.y > aabb2.max.y) {
            else if (aabb1.min.GetY() > aabb2.max.GetY()) {
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
//bool CollisionSystemECS::AABBSlopeCollision(Entity platform, Entity player, myMath::Vector2D velocity)
//{
//    //CollisionSystemECS::AABB playerAABB = {
//    //    { ecsCoordinator.getComponent<TransformComponent>(player).position.x - ecsCoordinator.getComponent<TransformComponent>(player).scale.x * 0.5f,
//    //      ecsCoordinator.getComponent<TransformComponent>(player).position.y - ecsCoordinator.getComponent<TransformComponent>(player).scale.y * 0.5f },
//    //    { ecsCoordinator.getComponent<TransformComponent>(player).position.x + ecsCoordinator.getComponent<TransformComponent>(player).scale.x * 0.5f,
//    //      ecsCoordinator.getComponent<TransformComponent>(player).position.y + ecsCoordinator.getComponent<TransformComponent>(player).scale.y * 0.5f }
//    //};
//
//    CollisionSystemECS::AABB playerAABB = {
//    { ecsCoordinator.getComponent<TransformComponent>(player).position.GetX() - ecsCoordinator.getComponent<TransformComponent>(player).scale.GetX() * 0.5f,
//      ecsCoordinator.getComponent<TransformComponent>(player).position.GetY() - ecsCoordinator.getComponent<TransformComponent>(player).scale.GetY() * 0.5f},
//    { ecsCoordinator.getComponent<TransformComponent>(player).position.GetX() + ecsCoordinator.getComponent<TransformComponent>(player).scale.GetX() * 0.5f,
//      ecsCoordinator.getComponent<TransformComponent>(player).position.GetY() + ecsCoordinator.getComponent<TransformComponent>(player).scale.GetY() * 0.5f }
//    };
//
//    // Define platform's AABB (before rotation)
//    CollisionSystemECS::AABB platformAABB = {
//        { ecsCoordinator.getComponent<AABBComponent>(platform).left, ecsCoordinator.getComponent<AABBComponent>(platform).bottom },
//        { ecsCoordinator.getComponent<AABBComponent>(platform).right, ecsCoordinator.getComponent<AABBComponent>(platform).top }
//    };
//
//    // Compute the sine and cosine of the platform's rotation angle
//    //float radians = ecsCoordinator.getComponent<TransformComponent>(platform).orientation.x * (M_PI / 180.f);
//    float radians = ecsCoordinator.getComponent<TransformComponent>(platform).orientation.GetX() * (M_PI / 180.f);
//    float s = sin(-radians);
//    float c = cos(-radians);
//
//    // Translate the player's AABB to the platform's local space (relative to platform's origin)
///*    glm::vec2 translatedMin = { playerAABB.min.x - ecsCoordinator.getComponent<TransformComponent>(platform).position.x,
//                                playerAABB.min.y - ecsCoordinator.getComponent<TransformComponent>(platform).position.y }; */   
//    myMath::Vector2D translatedMin = { playerAABB.min.GetX() - ecsCoordinator.getComponent<TransformComponent>(platform).position.GetX(),
//                                       playerAABB.min.GetY() - ecsCoordinator.getComponent<TransformComponent>(platform).position.GetY()};
//
//    //glm::vec2 translatedMax = { playerAABB.max.x - ecsCoordinator.getComponent<TransformComponent>(platform).position.x,
//    //                            playerAABB.max.y - ecsCoordinator.getComponent<TransformComponent>(platform).position.y };
//    myMath::Vector2D translatedMax = { playerAABB.max.GetX() - ecsCoordinator.getComponent<TransformComponent>(platform).position.GetX(),
//                                       playerAABB.max.GetY() - ecsCoordinator.getComponent<TransformComponent>(platform).position.GetY()};
//
//    // Rotate the translated AABB points (counter-clockwise to match platform rotation)
//    //glm::vec2 rotatedMin = {
//    //    translatedMin.x * c - translatedMin.y * s,
//    //    translatedMin.x * s + translatedMin.y * c
//    //};    
//    myMath::Vector2D rotatedMin = {
//        translatedMin.GetX() * c - translatedMin.GetY() * s,
//        translatedMin.GetX() * s + translatedMin.GetY() * c
//    };
//    //glm::vec2 rotatedMax = {
//    //    translatedMax.x * c - translatedMax.y * s,
//    //    translatedMax.x * s + translatedMax.y * c
//    //};    
//    myMath::Vector2D rotatedMax = {
//        translatedMax.GetX() * c - translatedMax.GetY() * s,
//        translatedMax.GetX() * s + translatedMax.GetY() * c
//    };
//
//    // Translate the rotated AABB back to world coordinates
//    //rotatedMin.x += ecsCoordinator.getComponent<TransformComponent>(platform).position.x;
//    //rotatedMin.y += ecsCoordinator.getComponent<TransformComponent>(platform).position.y;
//    //rotatedMax.x += ecsCoordinator.getComponent<TransformComponent>(platform).position.x;
//    //rotatedMax.y += ecsCoordinator.getComponent<TransformComponent>(platform).position.y;
//    rotatedMin.SetX(rotatedMin.GetX() + ecsCoordinator.getComponent<TransformComponent>(platform).position.GetX());
//    rotatedMin.SetY(rotatedMin.GetY() + ecsCoordinator.getComponent<TransformComponent>(platform).position.GetY());
//    rotatedMax.SetX(rotatedMax.GetX() + ecsCoordinator.getComponent<TransformComponent>(platform).position.GetX());
//    rotatedMax.SetY(rotatedMax.GetY() + ecsCoordinator.getComponent<TransformComponent>(platform).position.GetY());
//
//    // Create a rotated AABB for the player
//    //CollisionSystemECS::AABB rotatedPlayerAABB = {
//    //    { rotatedMin.x, rotatedMin.y },
//    //    { rotatedMax.x, rotatedMax.y }
//    //};    
//    CollisionSystemECS::AABB rotatedPlayerAABB = {
//        { rotatedMin.GetX(), rotatedMin.GetY()},
//        { rotatedMax.GetX(), rotatedMax.GetY()}
//    };
//
//    // Check for collision between the rotated player's AABB and the platform's AABB
//    float firstTimeOfCollision = 0;
//    bool collisionDetected = CollisionIntersection_RectRect(platformAABB, { 0, 0 }, rotatedPlayerAABB, velocity, firstTimeOfCollision);
//
//    //float angleRad = ecsCoordinator.getComponent<TransformComponent>(platform).orientation.x * (M_PI / 180.f);
//    float angleRad = ecsCoordinator.getComponent<TransformComponent>(platform).orientation.GetX() * (M_PI / 180.f);
//    myMath::Vector2D platformPos = ecsCoordinator.getComponent<TransformComponent>(platform).position;
//
//    // Rotate the circle's center relative to the platform
//    //float rotatedX = std::cos(angleRad) * (ecsCoordinator.getComponent<TransformComponent>(player).position.x - platformPos.x) + std::sin(angleRad) * (ecsCoordinator.getComponent<TransformComponent>(player).position.y - platformPos.y);
//    //float rotatedY = -std::sin(angleRad) * (ecsCoordinator.getComponent<TransformComponent>(player).position.x - platformPos.x) + std::cos(angleRad) * (ecsCoordinator.getComponent<TransformComponent>(player).position.y - platformPos.y);
//
//    float rotatedX = std::cos(angleRad) * (ecsCoordinator.getComponent<TransformComponent>(player).position.GetX() - platformPos.GetX()) + std::sin(angleRad) * (ecsCoordinator.getComponent<TransformComponent>(player).position.GetY() - platformPos.GetY());
//    float rotatedY = -std::sin(angleRad) * (ecsCoordinator.getComponent<TransformComponent>(player).position.GetX() - platformPos.GetX()) + std::cos(angleRad) * (ecsCoordinator.getComponent<TransformComponent>(player).position.GetY() - platformPos.GetY());
//
//    // Compute half extents of the platform along each axis (for AABB collision)
//    //glm::vec2 platformScl = ecsCoordinator.getComponent<TransformComponent>(platform).scale;
//    myMath::Vector2D platformScl = ecsCoordinator.getComponent<TransformComponent>(platform).scale;
//    float halfExtentX = platformScl.GetX() / 2.f;
//    float halfExtentY = platformScl.GetY() / 2.f;
//
//    float closestX = std::max(-halfExtentX, std::min(rotatedX, halfExtentX));
//    float closestY = std::max(-halfExtentY, std::min(rotatedY, halfExtentY));
//
//    float distanceX = rotatedX - closestX;
//    float distanceY = rotatedY - closestY;
//    float distanceSquared = distanceX * distanceX + distanceY * distanceY;
//
//    // Compute the distance from the circle center to the rectangle's center along each axis
//    float distX = std::abs(rotatedX) - halfExtentX;
//    float distY = std::abs(rotatedY) - halfExtentY;
//
//    if (distanceSquared <= 50 * 50) {
//        // Intersection detected, transform point back to world coordinates
//        //glm::vec2 intersectionPoint = {
//        //    platformPos.x + std::cos(-angleRad) * closestX - std::sin(-angleRad) * closestY,
//        //    platformPos.y + std::sin(angleRad) * closestX + std::cos(angleRad) * closestY
//        //};        
//        myMath::Vector2D intersectionPoint = {
//            platformPos.GetX() + std::cos(-angleRad) * closestX - std::sin(-angleRad) * closestY,
//            platformPos.GetY() + std::sin(angleRad) * closestX + std::cos(angleRad) * closestY
//        };
//
//        SetCollisionPoint(intersectionPoint);
//    }
//    return collisionDetected;
//}

// PROTOTYPING: Circle vs Rectangle collision detection
//CollisionSide CollisionSystemECS::circleRectCollision(float circleX, float circleY, float circleRadius, Entity platform) {
//    //float angleRad = ecsCoordinator.getComponent<TransformComponent>(platform).orientation.x * (M_PI / 180.f);
//    //glm::vec2 platformPos = ecsCoordinator.getComponent<TransformComponent>(platform).position;    
//    float angleRad = ecsCoordinator.getComponent<TransformComponent>(platform).orientation.GetX() * (M_PI / 180.f);
//    myMath::Vector2D platformPos = ecsCoordinator.getComponent<TransformComponent>(platform).position;
//
//    // Rotate the circle's center relative to the platform
//    //float rotatedX = std::cos(angleRad) * (circleX - platformPos.x) + std::sin(angleRad) * (circleY - platformPos.y);
//    //float rotatedY = -std::sin(angleRad) * (circleX - platformPos.x) + std::cos(angleRad) * (circleY - platformPos.y);
//    float rotatedX = std::cos(angleRad) * (circleX - platformPos.GetX()) + std::sin(angleRad) * (circleY - platformPos.GetY());
//    float rotatedY = -std::sin(angleRad) * (circleX - platformPos.GetX()) + std::cos(angleRad) * (circleY - platformPos.GetY());
//
//    // Compute half extents of the platform along each axis (for AABB collision)
//    //glm::vec2 platformScl = ecsCoordinator.getComponent<TransformComponent>(platform).scale;
//    myMath::Vector2D platformScl = ecsCoordinator.getComponent<TransformComponent>(platform).scale;
//    //float halfExtentX = platformScl.x / 2;
//    //float halfExtentY = platformScl.y / 2;
//    float halfExtentX = platformScl.GetX() / 2;
//    float halfExtentY = platformScl.GetY() / 2;
//
//    float closestX = std::max(-halfExtentX, std::min(rotatedX, halfExtentX));
//    float closestY = std::max(-halfExtentY, std::min(rotatedY, halfExtentY));
//
//    float distanceX = rotatedX - closestX;
//    float distanceY = rotatedY - closestY;
//    float distanceSquared = distanceX * distanceX + distanceY * distanceY;
//
//    // Compute the distance from the circle center to the rectangle's center along each axis
//    float distX = std::abs(rotatedX) - halfExtentX;
//    float distY = std::abs(rotatedY) - halfExtentY;
//
//    if (distanceSquared <= circleRadius * circleRadius) {
//        // Intersection detected, transform point back to world coordinates
//        //float intersectionX = platformPos.x + std::cos(-angleRad) * closestX - std::sin(-angleRad) * closestY;
//        //float intersectionY = platformPos.y + std::sin(angleRad) * closestX + std::cos(angleRad) * closestY;        
//        float intersectionX = platformPos.GetX() + std::cos(-angleRad) * closestX - std::sin(-angleRad) * closestY;
//        float intersectionY = platformPos.GetY() + std::sin(angleRad) * closestX + std::cos(angleRad) * closestY;
//
//        SetCollisionPoint({ intersectionX, intersectionY });
//    }
//
//    // If the distance is negative, the circle's center is within the rectangle along that axis
//    distX = (distX < 0) ? 0 : distX;
//    distY = (distY < 0) ? 0 : distY;
//
//    // Check for collision
//    if (distX * distX + distY * distY <= circleRadius * circleRadius) {
//        // Determine collision side based on distance and circle position
//        if (rotatedY > halfExtentY) {
//            if (rotatedX > halfExtentX) {
//                // Collision from the top-right corner
//                std::cout << "TR" << std::endl;
//                return CollisionSide::RIGHTEDGE;
//            }
//            else if (rotatedX < -halfExtentX) {
//                // Collision from the top-left corner
//                std::cout << "TL" << std::endl;
//                return CollisionSide::LEFTEDGE;
//            }
//            else {
//                // Collision from the top side
//                std::cout << "T" << std::endl;
//                return CollisionSide::TOP;
//            }
//        }
//        else {
//            if (rotatedX > halfExtentX) {
//                // Collision from the right side
//                std::cout << "R" << std::endl;
//                return CollisionSide::RIGHT;
//            }
//            else if (rotatedX < -halfExtentX) {
//                // Collision from the left side
//                std::cout << "L" << std::endl;
//                return CollisionSide::LEFT;
//            }
//            else {
//                // Collision from the bottom side
//                std::cout << "B" << std::endl;
//                return CollisionSide::BOTTOM;
//            }
// 
//    }
//
//    // If no collision was detected, return NONE
//    return CollisionSide::NONE;
//}

// Update function for Physics System
void PhysicsSystemECS::update(float dt) {
    //let it be the first entity
    Entity playerEntity = ecsCoordinator.getFirstEntity();
    Entity closestPlatformEntity = ecsCoordinator.getFirstEntity();

    playerEntity = ecsCoordinator.getEntityFromID("player");

    myMath::Vector2D velocity = ecsCoordinator.getComponent<RigidBodyComponent>(playerEntity).velocity;

    //if (GetVelocity().y < 0 && !isFalling)
    if (velocity.GetY() < 0 && !isFalling)
    {
        isFalling = true;
        eventSource.NotifyFall(playerEntity);
    }

    //if (GetVelocity().y >= 0 && isFalling)
    if (velocity.GetY() >= 0 && isFalling)
    {
        isFalling = false;
    }

    //if (GetVelocity().y > 0 && !alrJumped)
    if (velocity.GetY() > 0 && !alrJumped)
    {
        alrJumped = true;
        eventSource.NotifyJump(playerEntity);
    }

    //if (GetVelocity().y <= 0 && alrJumped)
    if (velocity.GetY() <= 0 && alrJumped)
    {
        alrJumped = false;
    }
    //HandlePlayerInput(playerEntity);

    closestPlatformEntity = FindClosestPlatform(playerEntity);
    //float slopeAngle = ecsCoordinator.getComponent<TransformComponent>(closestPlatformEntity).orientation.x;
    //if (slopeAngle != 0) {
    //    HandleOBBCircleCollision(closestPlatformEntity, playerEntity);
    //}
    //else {
    //    HandleAABBCollision(playerEntity, closestPlatformEntity);  // Use AABB for horizontal platforms
    //}
    //HandleOBBCircleCollision(closestPlatformEntity, playerEntity);
    HandleCircleOBBCollision(playerEntity, closestPlatformEntity);
    //HandleAABBCollision(playerEntity, closestPlatformEntity);



    //ecsCoordinator.getComponent<TransformComponent>(playerEntity).position.x += GetVelocity().x;
    //ecsCoordinator.getComponent<TransformComponent>(playerEntity).position.y += GetVelocity().y;    
    //ecsCoordinator.getComponent<TransformComponent>(playerEntity).position.SetX(ecsCoordinator.getComponent<TransformComponent>(playerEntity).position.GetX() + velocity.GetX());
    //ecsCoordinator.getComponent<TransformComponent>(playerEntity).position.SetY(ecsCoordinator.getComponent<TransformComponent>(playerEntity).position.GetY() + velocity.GetY());

}

std::string PhysicsSystemECS::getSystemECS() {
    return "PhysicsColliSystemECS";
}