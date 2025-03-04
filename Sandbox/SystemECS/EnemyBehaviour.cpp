/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   EnemyBehaviour.cpp
@brief:  This source file includes the implementation of the EnemyBehaviour
         that logicSystemECS uses to handle the behaviour of the Enemy entities.
         Note that in the enum there are 3 states. However, only the patrol state
         is implemented. The other states will be defined in future implementations.
         Waypoints are also current set to a fixed path for all enemies.

         Joel Chu (c.weiyuan): defined the functions of EnemyBehaviour class
                               100%
*//*___________________________________________________________________________-*/

#include "EnemyBehaviour.h"
#include "GlobalCoordinator.h"
#include "PhyColliSystemECS.h"

EnemyBehaviour::EnemyBehaviour() {
	currentState = PATROL;

	//For now all enemies have same way point
	waypoints.push_back(myMath::Vector2D(400, 200));
	waypoints.push_back(myMath::Vector2D(300, 200));
	waypoints.push_back(myMath::Vector2D(300, 100));
	waypoints.push_back(myMath::Vector2D(400, 100));

}

void EnemyBehaviour::switchState(STATE newState) {  
   currentState = newState;  
}

void EnemyBehaviour::update(Entity entity) {
    auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);

	myMath::Vector2D velocity = ecsCoordinator.getComponent<PhysicsComponent>(entity).velocity;

	if ((*GLFWFunctions::keyState)[Key::LEFT]) {
		transform.orientation.SetY(transform.orientation.GetY() + (180.f * GLFWFunctions::delta_time));
	}
	else if ((*GLFWFunctions::keyState)[Key::RIGHT]) {
		transform.orientation.SetY(transform.orientation.GetY() - (180.0f * GLFWFunctions::delta_time));
	}

	if ((*GLFWFunctions::keyState)[Key::UP]) {
		if (transform.scale.GetX() < 500.0f && transform.scale.GetY() < 500.0f) {
			transform.scale.SetX(transform.scale.GetX() + 53.4f * GLFWFunctions::delta_time);
			transform.scale.SetY(transform.scale.GetY() + 30.0f * GLFWFunctions::delta_time);
		}
	}
	else if ((*GLFWFunctions::keyState)[Key::DOWN]) {
		if (transform.scale.GetX() > 100.0f && transform.scale.GetY() > 100.0f) {
			transform.scale.SetX(transform.scale.GetX() - 53.4f * GLFWFunctions::delta_time);
			transform.scale.SetY(transform.scale.GetY() - 30.0f * GLFWFunctions::delta_time);
		}
	}


	switch (currentState) {
	case PATROL:
		updatePatrolState(entity);
		break;
	case CHASE:
		break;
	case ATTACK:
		break;
	}
}

std::vector<myMath::Vector2D>& EnemyBehaviour::getWaypoints() {
	return waypoints;
}

int& EnemyBehaviour::getCurrentWaypointIndex() {
	return currentWaypointIndex;
}

void EnemyBehaviour::updatePatrolState(Entity entity) {
    auto PhysicsSystemRef = ecsCoordinator.getSpecificSystem<PhysicsSystemECS>();
    auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
    auto& physics = ecsCoordinator.getComponent<PhysicsComponent>(entity);
    auto& currentWaypoints = getWaypoints();
    int& currentWPIndex = getCurrentWaypointIndex();
    ForceManager forceManager = ecsCoordinator.getComponent<PhysicsComponent>(entity).forceManager;

    // Set the current waypoint target
    myMath::Vector2D target = currentWaypoints[currentWPIndex];

    // Compute direction towards the target
    myMath::Vector2D direction = target - transform.position;
    float length = static_cast<float>(std::sqrt(std::pow(direction.GetX(), 2) + std::pow(direction.GetY(), 2)));

    // If close enough to the waypoint, move to the next one
    const float waypointThreshold = 1.0f;
    if (length < waypointThreshold) {
        // Snap position to the exact target
        transform.position.SetX(target.GetX());
        transform.position.SetY(target.GetY());

        // Reset forces and velocity
        physics.accumulatedForce.SetX(0.f);
        physics.accumulatedForce.SetY(0.f);
        physics.velocity.SetX(0.f);
        physics.velocity.SetY(0.f);

        // Move to the next waypoint
        currentWPIndex++;
        if (currentWPIndex >= currentWaypoints.size()) {
            currentWPIndex = 0; // Loop back to the first waypoint
        }

        target = currentWaypoints[currentWPIndex];
        direction = target - transform.position;
        length = static_cast<float>(std::sqrt(std::pow(direction.GetX(), 2) + std::pow(direction.GetY(), 2)));

    }

    // Normalize direction if length is not zero
    if (length != 0) {
        direction.SetX(direction.GetX() / static_cast<float>(length));
        direction.SetY(direction.GetY() / static_cast<float>(length));
    }

    // Apply movement force if not at the waypoint
    if (length >= waypointThreshold) {
        const float movementForceMagnitude = 5.0f; // Example value
        myMath::Vector2D movementForce = direction * movementForceMagnitude;
        forceManager.AddForce(entity, movementForce);
    }

    // Physics calculations
    float invMass = physics.mass > 0.f ? 1.f / physics.mass : 0.f;
    physics.acceleration = physics.accumulatedForce * invMass;

    // Update velocity
    physics.velocity.SetX(physics.velocity.GetX() + physics.acceleration.GetX() * GLFWFunctions::delta_time);
    physics.velocity.SetY(physics.velocity.GetY() + physics.acceleration.GetY() * GLFWFunctions::delta_time);

    const float maxSpeed = 0.2f; // Example max speed
    if (physics.velocity.GetX() > maxSpeed) physics.velocity.SetX(maxSpeed);
    if (physics.velocity.GetX() < -maxSpeed) physics.velocity.SetX(-maxSpeed);
    if (physics.velocity.GetY() > maxSpeed) physics.velocity.SetY(maxSpeed);
    if (physics.velocity.GetY() < -maxSpeed) physics.velocity.SetY(-maxSpeed);

    // Apply velocity to position
    transform.position.SetX(transform.position.GetX() + physics.velocity.GetX());
    transform.position.SetY(transform.position.GetY() + physics.velocity.GetY());

}
