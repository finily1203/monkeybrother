/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Lee Jing Wen (jingwen.lee)
@team:   MonkeHood
@course: CSD2401
@file:   PhyColliSystemECS.cpp
@brief:  This source file contains the implementation of the Physics and Collision
         System for ECS. Handling collision response accordingly. Also, update
         the physics of the player entity.
         Lee Jing Wen (jingwen.lee): Declared thePhysics System, Collision System
                                     (OBB), handling collision and integration
                                     with ECS. Loading of physics config from JSON.
                                     100%
*//*____________________________________________________________________________-*/


#include "ECSCoordinator.h"
#include "PhyColliSystemECS.h"
#include "TransformComponent.h"
#include "AABBComponent.h"
#include "MovementComponent.h"
#include "ClosestPlatform.h"
#include "GraphicsComponent.h"
#include "PhysicsComponent.h"
#include "PlayerComponent.h"

#include "GlobalCoordinator.h"
#include "GraphicsSystem.h"
#include <unordered_set>
#include "AudioSystem.h"

#define M_PI   3.14159265358979323846264338327950288f

float PhysicsSystemECS::friction;
float PhysicsSystemECS::threshold;
bool PhysicsSystemECS::alrJumped;
bool PhysicsSystemECS::isFalling;
bool PhysicsSystemECS::isSliding;

// PHYSICS SYSTEM

// Constructor for Physics System
PhysicsSystemECS::PhysicsSystemECS() : eventSource("PlayerEventSource"), eventObserver(std::make_shared<PlayerActionListener>())
{
    isColliding = false;
    eventSource.Register(MessageId::FALL, eventObserver);
    eventSource.Register(MessageId::JUMP, eventObserver);
}

void PhysicsSystemECS::initialise()
{
    LoadPhysicsConfigFromJSON(FilePathManager::GetPhysicsPath());
}

void PhysicsSystemECS::cleanup() {
    eventSource.Unregister(MessageId::FALL, eventObserver);
    eventSource.Unregister(MessageId::JUMP, eventObserver);
}

// Find the closest platform to the player
Entity PhysicsSystemECS::FindClosestPlatform(Entity player)
{
    float closestDistance = 100000.f;  // Initialize to a large number

    myMath::Vector2D playerPos = ecsCoordinator.getComponent<TransformComponent>(player).position;
    int count = 0;
    int isClosest = 0;
    Entity closestPlatform = player;
    for (auto& platform : entities)
    {
        if (platform == player)
        {
            continue;
        }
        bool hasClosestPlatform = ecsCoordinator.hasComponent<ClosestPlatform>(platform);

        if (hasClosestPlatform)
        {
            count++;
            myMath::Vector2D platformPos = ecsCoordinator.getComponent<TransformComponent>(platform).position;           
            float distance = std::sqrt(
                std::pow(playerPos.GetX() - platformPos.GetX(), 2.f) +
                std::pow(playerPos.GetY() - platformPos.GetY(), 2.f)
            ); 

            if (distance < closestDistance)
            {
                closestDistance = distance;
                ecsCoordinator.getComponent<ClosestPlatform>(platform).isClosest = true;
                isClosest = count;
                closestPlatform = platform;
            }
        }
    }

    return closestPlatform;
}

// Clamp the player's velocity
void PhysicsSystemECS::clampVelocity(Entity player, float maxVelocity) {
    myMath::Vector2D& velocity = ecsCoordinator.getComponent<PhysicsComponent>(player).velocity;
    float speed = myMath::LengthVector2D(velocity);

    if (speed > maxVelocity)
    {
        myMath::NormalizeVector2D(velocity, velocity);
        velocity = velocity * maxVelocity;
    }
}

// Calculate the directional vector based on the orientation of player
myMath::Vector2D PhysicsSystemECS::directionalVector(float angle)
{
    myMath::Vector2D vector;
    float adjustedAngle = (angle - 90.0f) * (M_PI / 180.0f);

    float cosValue = cos(adjustedAngle);
    float sinValue = sin(adjustedAngle);

    if (adjustedAngle == M_PI / 2.f || adjustedAngle == -M_PI / 2.f)
    {
        cosValue = 0;
    }

    vector.SetX(cosValue);
    vector.SetY(sinValue);

    return vector;
}

// Add applied force to accumulatedForce
void ForceManager::AddForce(Entity player, const myMath::Vector2D& appliedForce)
{
    myMath::Vector2D& accForce = ecsCoordinator.getComponent<PhysicsComponent>(player).accumulatedForce;

    accForce.SetX(accForce.GetX() + appliedForce.GetX());
    accForce.SetY(accForce.GetY() + appliedForce.GetY());

}

// Calculate the resultant force
float ForceManager::ResultantForce(myMath::Vector2D direction, myMath::Vector2D normal, float maxAccForce)
{
    float dotProduct = myMath::DotProductVector2D(direction, -normal);
    float angle = std::acos(dotProduct); // Angle in radians

    float forceFactor = std::sin(angle); // Will be between 0 and 1

    return maxAccForce * forceFactor;
}

// Clear the force (Reset to 0
void ForceManager::ClearForce(Entity player) {
    myMath::Vector2D& accForce = ecsCoordinator.getComponent<PhysicsComponent>(player).accumulatedForce;
    accForce.SetX(0.f);
    accForce.SetY(0.f);
}

// Apply force to the player
void ForceManager::ApplyForce(Entity player, myMath::Vector2D direction, float targetForce)
{
    myMath::Vector2D& playerPos = ecsCoordinator.getComponent<TransformComponent>(player).position;

    myMath::Vector2D& vel = ecsCoordinator.getComponent<PhysicsComponent>(player).velocity;
    myMath::Vector2D& acceleration = ecsCoordinator.getComponent<PhysicsComponent>(player).acceleration;
    myMath::Vector2D& accForce = ecsCoordinator.getComponent<PhysicsComponent>(player).accumulatedForce;

    float mass = ecsCoordinator.getComponent<PhysicsComponent>(player).mass;
    float dampen = ecsCoordinator.getComponent<PhysicsComponent>(player).dampening;
    float maxVelocity = ecsCoordinator.getComponent<PhysicsComponent>(player).maxVelocity;
    float& prevForce = ecsCoordinator.getComponent<PhysicsComponent>(player).prevForce;

    if (prevForce != targetForce) {
        accForce.SetX(targetForce);
        accForce.SetY(targetForce);
    }

    float invMass = mass > 0.f ? 1.f / mass : 0.f;
    acceleration = accForce * invMass;

    vel.SetX(vel.GetX() + direction.GetX() * acceleration.GetX());
    vel.SetY(vel.GetY() + direction.GetY() * acceleration.GetY());

    Console::GetLog() << "force:" << acceleration.GetX() << " " << acceleration.GetY() << std::endl;

    //Dampening
    vel.SetX(vel.GetX() * dampen);
    vel.SetY(vel.GetY() * dampen);

    float speed = myMath::LengthVector2D(vel);
    if (speed > maxVelocity) {
        myMath::NormalizeVector2D(vel, vel);
        vel = vel * maxVelocity;
    }

    ecsCoordinator.getSpecificSystem<PhysicsSystemECS>()->clampVelocity(player, maxVelocity);

    Console::GetLog() << "vel: " << vel.GetX() << " " << vel.GetY() << std::endl;

    playerPos.SetX(playerPos.GetX() + (vel.GetX() * GLFWFunctions::delta_time));
    playerPos.SetY(playerPos.GetY() + (vel.GetY() * GLFWFunctions::delta_time));
}

// Handle OBB collision
void PhysicsSystemECS::HandleCircleOBBCollision(Entity player, Entity platform)
{
    //myMath::Vector2D& playerPos = ecsCoordinator.getComponent<TransformComponent>(player).position;
    ////myMath::Vector2D& accForce          = ecsCoordinator.getComponent<PhysicsComponent>(player).accumulatedForce;
    //float radius = ecsCoordinator.getComponent<TransformComponent>(player).scale.GetX() * 0.5f;
    //float rotation = ecsCoordinator.getComponent<TransformComponent>(player).orientation.GetX();
    //myMath::Vector2D direction = directionalVector(rotation);
    //myMath::Vector2D gravity = ecsCoordinator.getComponent<PhysicsComponent>(player).gravityScale;
    //float mass = ecsCoordinator.getComponent<PhysicsComponent>(player).mass;
    //float maxAccForce = ecsCoordinator.getComponent<PhysicsComponent>(player).maxAccumulatedForce;
    //float& targetForce = ecsCoordinator.getComponent<PhysicsComponent>(player).targetForce;
    //float& prevForce = ecsCoordinator.getComponent<PhysicsComponent>(player).prevForce;
    //Force force = ecsCoordinator.getComponent<PhysicsComponent>(player).force;
    //CollisionSystemECS::OBB playerOBB = collisionSystem.createOBBFromEntity(player);
    //CollisionSystemECS::OBB platformOBB = collisionSystem.createOBBFromEntity(platform);

    //myMath::Vector2D normal{};
    //float penetration{};

    //force.SetDirection(direction);

    //isColliding = collisionSystem.checkCircleOBBCollision(playerPos, radius, platformOBB, normal, penetration);

    //

    //forceManager.AddForce(player, gravity * mass * GLFWFunctions::delta_time);

    //if (isColliding)
    //{
    //    alrJumped = true;
    //    if (-normal.GetX() == force.GetDirection().GetX() && -normal.GetY() == force.GetDirection().GetY())
    //    {
    //        forceManager.ClearForce(player);
    //    }

    //    targetForce = forceManager.ResultantForce(force.GetDirection(), normal, maxAccForce);
    //}

    //forceManager.ApplyForce(player, force.GetDirection(), targetForce);

    //prevForce = targetForce;

    //if (isColliding)
    //{
    //    if (GLFWFunctions::firstCollision == false)
    //    {
    //        GLFWFunctions::bumpAudio = true;
    //        GLFWFunctions::firstCollision = true;
    //        std::cout << "First time collide with platform" << std::endl;
    //    }
    //    collisionSystem.CollisionResponse(player, normal, penetration);
    //}
    //else
    //{
    //    GLFWFunctions::firstCollision = false;
    //}


}


// COLLISION SYSTEM
// OBB collision detection
// SAT for OBB vs Circle
CollisionSystemECS::OBB CollisionSystemECS::createOBBFromEntity(Entity entity)
{
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
float CollisionSystemECS::projectPoint(const myMath::Vector2D& point, const myMath::Vector2D& axis)
{
    return myMath::DotProductVector2D(point, axis);
}

// Get projection interval of OBB onto axis
void CollisionSystemECS::projectOBB(const OBB& obb, const myMath::Vector2D& axis, float& min, float& max)
{
    myMath::Vector2D vertices[4];
    getOBBVertices(obb, vertices);

    min = max = projectPoint(vertices[0], axis);
    for (int i = 1; i < 4; i++)
    {
        float projection = projectPoint(vertices[i], axis);
        min = std::min(min, projection);
        max = std::max(max, projection);
    }
}

// Get OBB vertices
void CollisionSystemECS::getOBBVertices(const OBB& obb, myMath::Vector2D vertices[4])
{
    vertices[0] = obb.center + obb.axes[0] * obb.halfExtents.GetX() + obb.axes[1] * obb.halfExtents.GetY();
    vertices[1] = obb.center - obb.axes[0] * obb.halfExtents.GetX() + obb.axes[1] * obb.halfExtents.GetY();
    vertices[2] = obb.center - obb.axes[0] * obb.halfExtents.GetX() - obb.axes[1] * obb.halfExtents.GetY();
    vertices[3] = obb.center + obb.axes[0] * obb.halfExtents.GetX() - obb.axes[1] * obb.halfExtents.GetY();
}

// Circle vs OBB collision detection using SAT
bool CollisionSystemECS::checkCircleOBBCollision(const myMath::Vector2D& circleCenter, float radius, const OBB& obb, myMath::Vector2D& normal, float& penetration)
{
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

    if (distSqr <= radius * radius)
    {
        float dist = sqrt(distSqr);
        normal = dist > 0 ? diff / dist : myMath::Vector2D(0, 1);

        penetration = radius - dist;
        return true;
    }

    return false;
}

// OBB vs OBB collision detection using SAT (CAN DETECT OBB COLLISION)
bool CollisionSystemECS::checkOBBCollisionSAT(const OBB& obb1, const OBB& obb2, myMath::Vector2D& normal, float& penetration)
{
    // Test axes from both OBBs
    const myMath::Vector2D* axes[] =
    {
        &obb1.axes[0], &obb1.axes[1],
        &obb2.axes[0], &obb2.axes[1]
    };

    for (int i = 0; i < 4; i++)
    {
        float min1, max1, min2, max2;
        projectOBB(obb1, *axes[i], min1, max1);
        projectOBB(obb2, *axes[i], min2, max2);

        // Check for separation
        if (min1 > max2 || min2 > max1)
        {
            return false;
        }

        float axisDepth = std::min(max2 - min1, max1 - min2);
        if (axisDepth < penetration)
        {
            penetration = axisDepth;
            normal = *axes[i];

            // Ensure normal points from obb1 to obb2
            myMath::Vector2D centerDiff = obb2.center - obb1.center;
            if (myMath::DotProductVector2D(centerDiff, normal) < 0)
            {
                normal = -normal;
            }
        }
    }
    return true;
}

// Collision response for OBB
void CollisionSystemECS::CollisionResponse(Entity player, myMath::Vector2D normal, float penetration)
{
    myMath::Vector2D& playerPos = ecsCoordinator.getComponent<TransformComponent>(player).position;
    myMath::Vector2D& vel = ecsCoordinator.getComponent<PhysicsComponent>(player).velocity;

    myMath::Vector2D tangent(-normal.GetY(), normal.GetX()); // Tangent vector along platform
    float tangentVelocity = myMath::DotProductVector2D(vel, tangent); // Velocity along tangent
    vel = tangent * tangentVelocity;

    playerPos.SetX(playerPos.GetX() + normal.GetX() * penetration);
    playerPos.SetY(playerPos.GetY() + normal.GetY() * penetration);
}

int count = 0;
Entity playerEntity = {};/* = ecsCoordinator.getFirstEntity();*/
Entity closestPlatformEntity = {};

// Update function for Physics System
void PhysicsSystemECS::update(float dt)
{
    (void)dt;

    //Entity closestPlatformEntity = ecsCoordinator.getFirstEntity();
    count = 0;
    for (auto& entity : entities)
    {
        if (ecsCoordinator.hasComponent<PlayerComponent>(entity)) {
            playerEntity = entity;
            count++;
        }

        if (ecsCoordinator.hasComponent<ClosestPlatform>(entity))
        {
            count++;
        }
    }

    if (count > 1)
    {
        closestPlatformEntity = FindClosestPlatform(playerEntity);
        HandleCircleOBBCollision(playerEntity, closestPlatformEntity);
    }

    std::vector<Entity> collidingPlatforms;
    // Count entities and find the player entity
    Entity playerEntity = NULL;
    for (auto& entity : entities)
    {
        if (ecsCoordinator.hasComponent<PlayerComponent>(entity))
        {
            playerEntity = entity;
        }
    }

    // Early exit if no player is found
    if (playerEntity == NULL) return;

    // Get the player position and radius
    auto& playerTransform = ecsCoordinator.getComponent<TransformComponent>(playerEntity);
    myMath::Vector2D playerPos = playerTransform.position;
    float playerRadius = playerTransform.scale.GetX() * 0.5f;

    // Detect all colliding platforms
    for (auto& entity : entities)
    {
        if (ecsCoordinator.hasComponent<ClosestPlatform>(entity))
        {
            auto& platformTransform = ecsCoordinator.getComponent<TransformComponent>(entity);

            // Check for collision between player and platform
            CollisionSystemECS::OBB platformOBB = collisionSystem.createOBBFromEntity(entity);
            myMath::Vector2D normal{};
            float penetration{};
            bool isColliding = collisionSystem.checkCircleOBBCollision(playerPos, playerRadius, platformOBB, normal, penetration);

            if (isColliding)
            {
                collidingPlatforms.push_back(entity);
            }
        }
    }

    // Resolve collisions
    if (collidingPlatforms.size() > 1)
    {
        for (auto& platformEntity : collidingPlatforms)
        {
            HandleCircleOBBCollision(playerEntity, platformEntity);
        }
    }



}

// Load physics config from JSON
void PhysicsSystemECS::LoadPhysicsConfigFromJSON(std::string const& filename)
{
    JSONSerializer serializer;

    // checks if the JSON file can be opened
    if (!serializer.Open(filename))
    {
        Console::GetLog() << "Error: could not open file " << filename << std::endl;
        return;
    }

    // retrieve the JSON object from the JSON file
    nlohmann::json currentObj = serializer.GetJSONObject();

    // read all of the data from the JSON object, assign every read
    // data to every elements that needs to be initialized
    serializer.ReadFloat(friction, "physics.friction");
    serializer.ReadFloat(threshold, "physics.threshold");
    serializer.ReadBool(alrJumped, "physics.alrJumped");
    serializer.ReadBool(isFalling, "physics.isFalling");
    serializer.ReadBool(isSliding, "physics.isSliding");

}

// Save physics config to JSON
void PhysicsSystemECS::SavePhysicsConfigFromJSON(std::string const& filename)
{
    JSONSerializer serializer;

    // checks if the JSON file can be opened
    if (!serializer.Open(filename))
    {
        Console::GetLog() << "Error: could not open file " << filename << std::endl;
        return;
    }

    // retrieve the JSON object from the JSON file
    nlohmann::json jsonObj = serializer.GetJSONObject();

    // read all of the data from the JSON object, assign every read
    // data to every elements that needs to be initialized
    serializer.WriteFloat(friction, "physics.friction", filename);
    serializer.WriteFloat(threshold, "physics.threshold", filename);
    serializer.WriteBool(alrJumped, "physics.alrJumped", filename);
    serializer.WriteBool(isFalling, "physics.isFalling", filename);
    serializer.WriteBool(isSliding, "physics.isSliding", filename);

}


// Get the system ECS
std::string PhysicsSystemECS::getSystemECS()
{
    return "PhysicsColliSystemECS";
}