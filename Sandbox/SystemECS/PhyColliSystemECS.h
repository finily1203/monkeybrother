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
    
    void CollisionResponse(Entity player, myMath::Vector2D normal, float penetration);

    //bool GetCheckCollisionFirstTime() const { return checkCollisionFirstTime; }
    //void SetCheckCollisionFirstTime(bool newCheckCollisionFirstTime) { checkCollisionFirstTime = newCheckCollisionFirstTime; }

    //bool GetCheckSlanted() const { return checkSlanted; }
    //void SetCheckSlanted(bool newCheckSlanted) { checkSlanted = newCheckSlanted; }

    //bool GetSlantedPlatformFirstTime() const { return slantedPlatformFirstTime; }
    //void SetSlantedPlatformFirstTime(bool newSlantedPlatformFirstTime) { slantedPlatformFirstTime = newSlantedPlatformFirstTime; }
private:
    myMath::Vector2D collisionPoint;
    //static bool checkCollisionFirstTime; 
    //static bool checkSlanted;
    //static bool slantedPlatformFirstTime;
};

class Force
{
public:

    Force(myMath::Vector2D direction, float magnitude) : direction(direction), magnitude(magnitude) {}

	myMath::Vector2D GetDirection() const { return direction; }
	void SetDirection(myMath::Vector2D newDirection) { direction = newDirection; }

	float GetMagnitude() const { return magnitude; }
	void SetMagnitude(float newMagnitude) { magnitude = newMagnitude; }

private:
    myMath::Vector2D direction;
    float magnitude;
};

class ForceManager
{
public:
    void AddForce(Entity player, const myMath::Vector2D& appliedForce);
    void ClearForce(Entity player);
    void ApplyForce(Entity player, myMath::Vector2D direction, float magnitude);

    float ResultantForce(myMath::Vector2D direction, myMath::Vector2D normal, float maxAccForce);
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
    myMath::Vector2D clampVelocity(myMath::Vector2D velocity, float maxVelocity);

    void LoadPhysicsConfigFromJSON(std::string const& filename);
    void SavePhysicsConfigFromJSON(std::string const& filename);

    ForceManager getForceManager() const { return forceManager; }

    // MILESTONE 1
    //// Handling slope collision for the player
    //void HandleSlopeCollision(Entity closestPlatform, Entity player);
    //// Handling AABB collision for the player
    //void HandleAABBCollision(Entity player, Entity closestPlatform);
    // Player input handling for movement (left: 'A', right: 'D')

    void HandlePlayerInput(Entity player);
    void ApplyGravity(Entity entity, float dt);

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
    Force Force;
};
