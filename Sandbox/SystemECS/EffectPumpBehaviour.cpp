/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   EffectPumpBehaviour.cpp
@brief:  This source file includes the implementation of the EffectPumpBehaviour
		 that logicSystemECS uses to handle the behaviour of the pump entities.
		 Pump entities will push the player entity and has a on and off duration

		 Joel Chu (c.weiyuan): defined the functions of EffectPumpBehaviour class
							   100%
*//*___________________________________________________________________________-*/

#include "EffectPumpBehaviour.h"
#include "LogicSystemECS.h"
#include "GlobalCoordinator.h"
#include "PhyColliSystemECS.h"

	
void EffectPumpBehaviour::update(Entity entity) {
    timer += GLFWFunctions::delta_time;
    if (GLFWFunctions::isPumpOn && timer >= onDuration) {
        GLFWFunctions::isPumpOn = false;
        timer = 0.0f;
        std::cout << "Pump off" << std::endl;
    }
    else if (!GLFWFunctions::isPumpOn && timer >= offDuration) {
        GLFWFunctions::isPumpOn = true;
        timer = 0.0f;
        std::cout << "Pump on" << std::endl;
    }
    if (GLFWFunctions::isPumpOn) {
        auto playerEntity = ecsCoordinator.getEntityFromID("player");
        auto PhysicsSystemRef = ecsCoordinator.getSpecificSystem<PhysicsSystemECS>();
        auto collisionSystem = PhysicsSystemRef->getCollisionSystem();
        auto& playerTransform = ecsCoordinator.getComponent<TransformComponent>(playerEntity);
        auto& bubblesTransform = ecsCoordinator.getComponent<TransformComponent>(entity);
        myMath::Vector2D& playerPos = playerTransform.position;
        float radius = playerTransform.scale.GetX() * 0.5f;
        CollisionSystemECS::OBB playerOBB = collisionSystem.createOBBFromEntity(playerEntity);
        CollisionSystemECS::OBB bubblesOBB = collisionSystem.createOBBFromEntity(entity);
        myMath::Vector2D normal{};
        float penetration{};
        bool isColliding = collisionSystem.checkCircleOBBCollision(playerPos, radius, bubblesOBB, normal, penetration);
        if (isColliding) {
            float pumpForce = ecsCoordinator.getComponent<PumpComponent>(entity).pumpForce;
            ForceManager& forceManager = ecsCoordinator.getComponent<PhysicsComponent>(playerEntity).forceManager;
            float orientation = bubblesTransform.orientation.GetX();

            myMath::Vector2D forceVector;

            // Handle different orientations
            if (orientation == 90.f) {  // Pointing up
                forceVector.SetX(-1.f);
                forceVector.SetY(0.f);
            }
            else if (orientation == 0.f) {  // Pointing right
                forceVector.SetX(0.f);
                forceVector.SetY(1.f);
            }
            else {  // For any other angle, use the directional vector
                myMath::Vector2D bubblesDirectionalVec = PhysicsSystemRef->directionalVector(orientation);
                forceVector = bubblesDirectionalVec;
            }

            forceManager.AddForce(playerEntity, forceVector * pumpForce);
            forceManager.ApplyForce(playerEntity, forceVector, pumpForce);

            std::cout << "Collision with pump - Force applied. Orientation: " << orientation << std::endl;
            std::cout << "Force vector: " << forceVector.GetX() << ", " << forceVector.GetY() << std::endl;
        }
    }
}