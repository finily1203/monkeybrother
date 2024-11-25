#include "EffectPumpBehaviour.h"
#include "LogicSystemECS.h"
#include "GlobalCoordinator.h"
#include "PhyColliSystemECS.h"

	
void EffectPumpBehaviour::update(Entity entity) {
	timer += GLFWFunctions::delta_time;
	if (isPumpOn && timer >= onDuration) {
		isPumpOn = false;
		timer = 0.0f;
		std::cout << "Pump off" << std::endl;
	}
	else if (!isPumpOn && timer >= offDuration) {
		isPumpOn = true;
		timer = 0.0f;
		std::cout << "Pump on" << std::endl;
	}

	if (isPumpOn) {
		auto playerEntity = ecsCoordinator.getEntityFromID("player");;
		auto PhysicsSystemRef = ecsCoordinator.getSpecificSystem<PhysicsSystemECS>();
		auto collisionSystem = PhysicsSystemRef->getCollisionSystem();

		auto& playerTransform = ecsCoordinator.getComponent<TransformComponent>(playerEntity);
		myMath::Vector2D& playerPos = playerTransform.position;
		float radius = playerTransform.scale.GetX() * 0.5f;

		CollisionSystemECS::OBB playerOBB = collisionSystem.createOBBFromEntity(playerEntity);
		CollisionSystemECS::OBB pumpOBB = collisionSystem.createOBBFromEntity(entity);

		myMath::Vector2D normal{};
		float penetration{};
		bool isColliding = collisionSystem.checkCircleOBBCollision(playerPos, radius, pumpOBB, normal, penetration);

		if (isColliding) {
			auto& playerPhysics = ecsCoordinator.getComponent<PhysicsComponent>(playerEntity);
			float mag = playerPhysics.force.GetMagnitude();

			PhysicsSystemRef->getForceManager().AddForce(playerEntity, myMath::Vector2D(-1.0f, -1.0f));

			std::cout << "Collision" << std::endl;
		}
	}
}