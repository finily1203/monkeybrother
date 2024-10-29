/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Lee Jing Wen (jingwen.lee)
@team: MonkeHood
@course: CSD2401
@file: PhyColliSystemECS.h
@brief: This header file contains the declaration of the Physics System and Collision System for ECS.
    Lee Jing Wen (jingwen.lee): Physics System, Collision System, AABB struct, enum CollisionSide
                  for identifying.
                  100%
*//*    _________________________________________________________________-*/
#pragma once
#include "ECSCoordinator.h"

enum CollisionSide {
    NONE,
    TOP,
    BOTTOM,
    LEFT,
    RIGHT,
    LEFTEDGE,
    RIGHTEDGE
};

class CollisionSystemECS
{
public:
    CollisionSystemECS() = default;

    struct AABB
    {
        glm::vec2 min;
        glm::vec2 max;
    };

    // AABB Collision detection
    bool CollisionIntersection_RectRect(const AABB& platform,
        const glm::vec2& platformVel,
        const AABB& player,
        const glm::vec2& playerVel,
        float& firstTimeOfCollision);

    // AABB slope collision detection
    bool AABBSlopeCollision(Entity platform, Entity player, glm::vec2 velocity);

    // Getters and Setters
    glm::vec2 GetCollisionPoint() const { return collisionPoint; }
    void SetCollisionPoint(glm::vec2 newCollisionPoint) { collisionPoint = newCollisionPoint; }

    // PROTOTYPING: Circle vs Rectangle collision detection
    CollisionSide circleRectCollision(float circleX, float circleY, float circleRadius, Entity platform);

private:
    glm::vec2 collisionPoint;

};

class PhysicsSystemECS : public System
{
public:
    PhysicsSystemECS();

    void initialise() override;
    void update(float dt) override;
    void cleanup() override;

    // Getters and Setters
    glm::vec2 GetVelocity() const { return velocity; }
    bool GetAlrJumped() const { return alrJumped; }

    void SetVelocity(glm::vec2 newVelocity) { velocity = newVelocity; }
    void SetAlrJumped(bool newAlrJumped) { alrJumped = newAlrJumped; }

    void ApplyGravity(Entity entity, float dt);
    Entity FindClosestPlatform(Entity player);

    // Handling slope collision for the player
    void HandleSlopeCollision(Entity closestPlatform, Entity player);

    // Handling AABB collision for the player
    void HandleAABBCollision(Entity player, Entity closestPlatform);

    // Player input handling for movement (left: 'A', right: 'D')
    void HandlePlayerInput(Entity player);

    // PROTYPING: Handling Circle vs Rectangle collision
    void HandleCircleCollision(Entity closestPlatform, Entity player);

    // PROTOTYPING: Handling Circle vs Rectangle side collision
    void HandleSideCollision(int collisionSide, float circleRadius, Entity player, Entity closestPlatform);

    // PROTOTYPING: Handling Circle vs Rectangle slope collision (sliding off)
    void HandleCircleSlopeCollision(Entity closestPlatform, Entity player);


private:
    glm::vec2 velocity;
    float gravity;
    float jumpForce;
    float friction;
    bool alrJumped;
    bool isFalling;
    Observable eventSource;
    std::shared_ptr<Observer> eventObserver;

    CollisionSystemECS collisionSystem;
};