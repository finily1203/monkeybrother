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

	std::cout << position.GetX() << " " << position.GetY() << std::endl;

	if (movement.movForward) {
		transform.position += direction * distance;
	}
	else {
		transform.position += -direction * distance;
	}

	//if it hits over the maxDistance, it will move back
	//calculate the distance between the start position and the current position
	//if it is greater than the maxDistance, move back

	float distTravelled = sqrt(pow(position.GetX() - movement.startPos.GetX(), 2) +
							   pow(position.GetY() - movement.startPos.GetY(), 2));
	

	if (distTravelled > movement.maxDistance)
	{
		movement.movForward = false;
	}

	if (position.GetX() < movement.startPos.GetX() || position.GetY() < movement.startPos.GetY())
	{
		movement.movForward = true;
	}
}