#include "PlatformBehaviour.h"
#include "GlobalCoordinator.h"
#include "PhyColliSystemECS.h"



void PlatformBehaviour::update(Entity entity) {
    auto PhysicsSystemRef = ecsCoordinator.getSpecificSystem<PhysicsSystemECS>();
    auto collisionSystem = PhysicsSystemRef->getCollisionSystem();
    bool isColliding = false;
    bool alrJumped;

    for (auto& playerEntity : ecsCoordinator.getAllLiveEntities()) {
        if (ecsCoordinator.hasComponent<PlayerComponent>(playerEntity)) {
            myMath::Vector2D& playerPos = ecsCoordinator.getComponent<TransformComponent>(playerEntity).position;
 
            float radius = ecsCoordinator.getComponent<TransformComponent>(playerEntity).scale.GetX() * 0.5f;
            float rotation = ecsCoordinator.getComponent<TransformComponent>(playerEntity).orientation.GetX();
            myMath::Vector2D direction = PhysicsSystemRef->directionalVector(rotation);
            myMath::Vector2D gravity = ecsCoordinator.getComponent<PhysicsComponent>(playerEntity).gravityScale;
            float mass = ecsCoordinator.getComponent<PhysicsComponent>(playerEntity).mass;
            float maxAccForce = ecsCoordinator.getComponent<PhysicsComponent>(playerEntity).maxAccumulatedForce;
            float& targetForce = ecsCoordinator.getComponent<PhysicsComponent>(playerEntity).targetForce;
            float& prevForce = ecsCoordinator.getComponent<PhysicsComponent>(playerEntity).prevForce;
            Force force = ecsCoordinator.getComponent<PhysicsComponent>(playerEntity).force;
            CollisionSystemECS::OBB playerOBB = collisionSystem.createOBBFromEntity(playerEntity);
            CollisionSystemECS::OBB platformOBB = collisionSystem.createOBBFromEntity(entity);

            myMath::Vector2D normal{};
            float penetration{};

            force.SetDirection(direction);

            isColliding = collisionSystem.checkCircleOBBCollision(playerPos, radius, platformOBB, normal, penetration);
            PhysicsSystemRef->getForceManager().AddForce(playerEntity, gravity * mass * GLFWFunctions::delta_time);

            if (isColliding)
            {
                alrJumped = true;
                if (-normal.GetX() == force.GetDirection().GetX() && -normal.GetY() == force.GetDirection().GetY())
                {
                    PhysicsSystemRef->getForceManager().ClearForce(playerEntity);
                }

                targetForce = PhysicsSystemRef->getForceManager().ResultantForce(force.GetDirection(), normal, maxAccForce);
            }

            PhysicsSystemRef->getForceManager().ApplyForce(playerEntity, force.GetDirection(), targetForce);

            prevForce = targetForce;

            if (isColliding)
            {
                collisionSystem.CollisionResponse(playerEntity, normal, penetration);
            }
            else
            {
                GLFWFunctions::firstCollision = false;
            }
        }
    }
    
}