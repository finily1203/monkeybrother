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

#define M_PI   3.14159265358979323846264338327950288f

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

        auto& physics = ecsCoordinator.getComponent<PhysicsComponent>(playerEntity);
        float rotation = bubblesTransform.orientation.GetX();   
        myMath::Vector2D direction = PhysicsSystemRef->directionalVector(rotation);

        Force force = ecsCoordinator.getComponent<PhysicsComponent>(playerEntity).force;
        ForceManager& forceManager = ecsCoordinator.getComponent<PhysicsComponent>(playerEntity).forceManager;

        float pumpForce = ecsCoordinator.getComponent<PumpComponent>(entity).pumpForce;
        myMath::Vector2D normal{};
        float penetration{};

        bool isColliding = collisionSystem.checkCircleOBBCollision(playerPos, radius, bubblesOBB, normal, penetration);
        if (isColliding) {
            //std::cout << playerTransform.orientation.GetX() << std::endl;

   //         force.SetDirection(direction);
   //         forceManager.AddForce(playerEntity, force.GetDirection() * pumpForce * GLFWFunctions::delta_time);
   //         forceManager.ApplyForce(playerEntity, force.GetDirection(), pumpForce);

   //         int playerOrientation = static_cast<int>(playerTransform.orientation.GetX()) % 360;
   //         float forceDirAngle = atan2(direction.GetY(), direction.GetX()) * (180.0f / M_PI);

   //         //playerOrientation = (playerOrientation + 360) % 360;
   //         //forceDirAngle = (static_cast<int>(forceDirAngle) + 360) % 360;

			//if (playerOrientation < 0) playerOrientation += 360;

   //         float angleDifference = fabs(playerOrientation - forceDirAngle);

   //         std::cout << "Player Orientation: " << playerTransform.orientation.GetX() << std::endl;
   //         std::cout << "Force Direction Angle: " << forceDirAngle << std::endl;

   //         forceManager.ClearForce(playerEntity);

            force.SetDirection(direction);
            forceManager.AddForce(playerEntity, force.GetDirection() * pumpForce * GLFWFunctions::delta_time);
            forceManager.ApplyForce(playerEntity, force.GetDirection(), pumpForce);

            int playerOrientation = static_cast<int>(playerTransform.orientation.GetX()) % 360;
            myMath::Vector2D PlayerDir = PhysicsSystemRef->directionalVector(playerOrientation);

			std::cout << PlayerDir.GetX() << ", " << PlayerDir.GetY() << std::endl;

            float threshold = 0.3f;

            //its kinda weird here but since pumpForce is -ve, use the -ve PlayerDir
			myMath::Vector2D dirDiff = direction - (-PlayerDir);

			std::cout << "Dir Diff: " << dirDiff.GetX() << ", " << dirDiff.GetY() << std::endl;

            if (dirDiff.GetX() < threshold)
            {
                forceManager.ClearForce(playerEntity);
            }
			if (dirDiff.GetY() < threshold)
			{
				forceManager.ClearForce(playerEntity);
			}
            
        }
    }
}