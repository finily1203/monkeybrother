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
#include "vector2D.h"

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
        myMath::Vector2D min;
        myMath::Vector2D max;
    };

    struct OBB {
        myMath::Vector2D center;      // Center position
        myMath::Vector2D halfExtents; // Half-width and half-height
        float rotation;        // Rotation in radians
        myMath::Vector2D axes[2];     // Local axes (normalized)
    };

    OBB createOBBFromEntity(Entity entity);

    // Project point onto axis
    float projectPoint(const myMath::Vector2D& point, const myMath::Vector2D& axis);
    // Get projection interval of OBB onto axis
    void projectOBB(const OBB& obb, const myMath::Vector2D& axis, float& min, float& max);

    // Get OBB vertices
    void getOBBVertices(const OBB& obb, myMath::Vector2D vertices[4]);
    // Circle vs OBB collision detection using SAT
    bool checkCircleOBBCollision(const myMath::Vector2D& circleCenter, float radius, const OBB& obb, myMath::Vector2D& normal, float& penetration);
    //bool checkOBBCollisionSAT(const OBB& obb1, const OBB& obb2);
    bool checkOBBCollisionSAT(const OBB& obb1, const OBB& obb2, myMath::Vector2D& normal, float& penetration);

    // AABB Collision detection
    bool CollisionIntersection_RectRect(const AABB& platform,
        const myMath::Vector2D& platformVel,
        const AABB& player,
        const myMath::Vector2D& playerVel,
        float& firstTimeOfCollision);

    // AABB slope collision detection
    //bool AABBSlopeCollision(Entity platform, Entity player, myMath::Vector2D velocity);

    // Getters and Setters
    myMath::Vector2D GetCollisionPoint() const { return collisionPoint; }
    void SetCollisionPoint(myMath::Vector2D newCollisionPoint) { collisionPoint = newCollisionPoint; }

    // PROTOTYPING: Circle vs Rectangle collision detection
    //CollisionSide circleRectCollision(float circleX, float circleY, float circleRadius, Entity platform);

private:
    myMath::Vector2D collisionPoint;

};


class PhysicsSystemECS : public System
{
public:
    PhysicsSystemECS();

    void initialise() override;
    void update(float dt) override;
    void cleanup() override;

    std::string getSystemECS() override;

    // Getters and Setters
    myMath::Vector2D GetVelocity() const { return velocity; }
    bool GetAlrJumped() const { return alrJumped; }

    void SetVelocity(myMath::Vector2D newVelocity) { velocity = newVelocity; }
    void SetAlrJumped(bool newAlrJumped) { alrJumped = newAlrJumped; }

    void ApplyForce(Entity player, const myMath::Vector2D& appliedForce);
    void ApplyGravity(Entity entity, float dt);
    Entity FindClosestPlatform(Entity player);

    void HandleCircleOBBCollision(Entity player, Entity platform);

    //// Handling slope collision for the player
    //void HandleSlopeCollision(Entity closestPlatform, Entity player);

    //// Handling AABB collision for the player
    //void HandleAABBCollision(Entity player, Entity closestPlatform);

    // Player input handling for movement (left: 'A', right: 'D')
    void HandlePlayerInput(Entity player);

    //// PROTYPING: Handling Circle vs Rectangle collision
    //void HandleCircleCollision(Entity closestPlatform, Entity player);

    //// PROTOTYPING: Handling Circle vs Rectangle side collision
    //void HandleSideCollision(int collisionSide, float circleRadius, Entity player, Entity closestPlatform);

    //// PROTOTYPING: Handling Circle vs Rectangle slope collision (sliding off)
    //void HandleCircleSlopeCollision(Entity closestPlatform, Entity player);


private:
    myMath::Vector2D velocity;
    float gravity;
    float jumpForce;
    float friction;
    bool alrJumped;
    bool isFalling;
    PlayerEventPublisher eventSource;
    std::shared_ptr<Observer> eventObserver;

    CollisionSystemECS collisionSystem;
};

//class ForcesSystemECS
//{
//public:
//    ForcesSystemECS();
//
//    //void ApplyForce(Entity entity, glm::vec2 force);
//    //void ApplyImpulse(Entity entity, glm::vec2 impulse);
//    //void ApplyFriction(Entity entity, float friction);
//    void ApplyGravity(Entity entity, float dt);
//    //void ApplyJumpForce(Entity entity, float jumpForce);
//    //void ApplyAcceleration(Entity entity, glm::vec2 acceleration);
//    //void ApplyMass(Entity entity, float mass);
//
//    float GetFriction() const { return friction; }
//    float GetGravity() const { return gravity; }
//    float GetJump() const { return jump; }
//    glm::vec2 GetAcceleration() const { return acceleration; }
//    float GetMass() const { return mass; }
//
//    void SetFriction(float newFriction) { friction = newFriction; }
//    void SetGravity(float newGravity) { gravity = newGravity; }
//    void SetJump(float newJump) { jump = newJump; }
//    void SetAcceleration(glm::vec2 newAcceleration) { acceleration = newAcceleration; }
//    void SetMass(float newMass) { mass = newMass; }
//
//    //Rotational Force
//    //void ApplyAngularVelocity(Entity entity, float angularVelocity);
//    //void ApplyAngularAcceleration(Entity entity, float angularAcceleration);
//    //void ApplyTorque(Entity entity, float torque);
//    //void ApplyImpulseAt(Entity entity, glm::vec2 impulse, glm::vec2 position);
//    //void ApplyTorqueAt(Entity entity, float torque, glm::vec2 position);
//    //void ApplyForceAt(Entity entity, glm::vec2 force, glm::vec2 position);
//    //void ApplyImpulseAtCenter(Entity entity, glm::vec2 impulse);
//    //void ApplyTorqueAtCenter(Entity entity, float torque);
//    //void ApplyForceAtCenter(Entity entity, glm::vec2 force);
//    //void ApplyImpulseAtLocalPoint(Entity entity, glm::vec2 impulse, glm::vec2 localPoint);
//    //void ApplyTorqueAtLocalPoint(Entity entity, float torque, glm::vec2 localPoint);
//    //void ApplyForceAtLocalPoint(Entity entity, glm::vec2 force, glm::vec2 localPoint);
//    //void ApplyImpulseAtWorldPoint(Entity entity, glm::vec2 impulse, glm::vec2 worldPoint);
//    //void ApplyTorqueAtWorldPoint(Entity entity, float torque, glm::vec2 worldPoint);
//    //void ApplyForceAtWorldPoint(Entity entity, glm::vec2 force, glm::vec2 worldPoint);
//    //void ApplyImpulseAtLocalCenter(Entity entity, glm::vec2 impulse);
//    //void ApplyTorqueAtLocalCenter(Entity entity, float torque);
//    //void ApplyForceAtLocalCenter(Entity entity, glm::vec2);
//private:
//    float friction;
//    float gravity;
//    float jump;
//    glm::vec2 acceleration;
//    float mass;
//};