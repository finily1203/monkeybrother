#include "MovPlatformBehaviour.h"
#include "LogicSystemECS.h"
#include "GlobalCoordinator.h"
#include "PhyColliSystemECS.h"

//Each platform with this behaviour will move at that direction at the speed given

void MovPlatformBehaviour::update(Entity entity) {
	auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
	auto& movement = ecsCoordinator.getComponent<MovPlatformComponent>(entity);
	myMath::Vector2D direction = movement.direction;
	float speed = movement.speed;
	myMath::Vector2D& position = transform.position;
	float distance = speed * 0.1f;

	if (movement.movForward) {
		transform.position += direction * distance;
	}
	else {
		transform.position += -direction * distance;
	}

	//if it hits over the maxDistance, it will move back
	//calculate the distance between the start position and the current position
	//if it is greater than the maxDistance, move back

	float distTravelled = (movement.startPos.GetX() - position.GetX()) * (movement.startPos.GetX() - position.GetX()) -
						  (movement.startPos.GetY() - position.GetY()) * (movement.startPos.GetY() - position.GetY());

	distTravelled = sqrt(distTravelled);

	if (distTravelled > movement.maxDistance)
	{
		movement.movForward = false;
	}
}