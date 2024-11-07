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
#include "EngineDefinitions.h"
#include "ECSCoordinator.h"
#include "vector2D.h"
#include "Force.h"

class CollisionSystemECS
{
public:
    CollisionSystemECS() = default;

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

    void CollisionResponse(Entity player, myMath::Vector2D normal, float penetration);
};

class PhysicsSystemECS : public System
{
public:
    PhysicsSystemECS();

    void initialise() override;
    void update(float dt) override;
    void cleanup() override;

    std::string getSystemECS() override;

    bool GetAlrJumped() const { return alrJumped; }
    void SetAlrJumped(bool newAlrJumped) { alrJumped = newAlrJumped; }

    Entity FindClosestPlatform(Entity player);
    void HandleCircleOBBCollision(Entity player, Entity platform);

    myMath::Vector2D directionalVector(float angle);
    void clampVelocity(Entity player, float maxVelocity);

    void LoadPhysicsConfigFromJSON(std::string const& filename);
    void SavePhysicsConfigFromJSON(std::string const& filename);

    ForceManager getForceManager() const { return forceManager; }

private:
    static float friction;
    static float threshold;
    static bool alrJumped;
    static bool isFalling;
    static bool isSliding;
    PlayerEventPublisher eventSource;
    std::shared_ptr<Observer> eventObserver;

    CollisionSystemECS collisionSystem;
    ForceManager forceManager;
};
