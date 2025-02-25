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
    isFacingRight = true;
	moveHorizontal = true;

	//For now all enemies have same way point
	//waypoints.push_back(myMath::Vector2D(400, 200));
	//waypoints.push_back(myMath::Vector2D(300, 200));
	//waypoints.push_back(myMath::Vector2D(300, 100));
     
	//waypoints.push_back(myMath::Vector2D(-200, 200)); // left top
 //   waypoints.push_back(myMath::Vector2D(-200, -200)); // left bottom
	//waypoints.push_back(myMath::Vector2D(200, -200)); // right bottom
	//waypoints.push_back(myMath::Vector2D(200, 200)); // right top

    //waypoints.push_back(myMath::Vector2D(-200, 200)); // left top
    //waypoints.push_back(myMath::Vector2D(200, 200)); // right top
    //waypoints.push_back(myMath::Vector2D(200, -200)); // right bottom
    //waypoints.push_back(myMath::Vector2D(-200, -200)); // left bottom

    //waypoints.push_back(myMath::Vector2D(-200, 200));
    //waypoints.push_back(myMath::Vector2D(200, 200));
    //waypoints.push_back(myMath::Vector2D(200, -200));
    //waypoints.push_back(myMath::Vector2D(-200, -200));
    //waypoints.push_back(myMath::Vector2D(-200, -100));
    //waypoints.push_back(myMath::Vector2D(-100, -100));
    //waypoints.push_back(myMath::Vector2D(-100, 100));
    //waypoints.push_back(myMath::Vector2D(-200, 100));

    waypoints.push_back(myMath::Vector2D(-200, 200));
	waypoints.push_back(myMath::Vector2D(-200, 100));
	waypoints.push_back(myMath::Vector2D(-100, 100));
	waypoints.push_back(myMath::Vector2D(-100, -100));
	waypoints.push_back(myMath::Vector2D(-200, -100));
	waypoints.push_back(myMath::Vector2D(-200, -200));
	waypoints.push_back(myMath::Vector2D(200, -200));
	waypoints.push_back(myMath::Vector2D(200, 200));

}

void EnemyBehaviour::switchState(STATE newState) {  
   currentState = newState;  
}

void EnemyBehaviour::update(Entity entity) {
    auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
	myMath::Vector2D velocity = ecsCoordinator.getComponent<PhysicsComponent>(entity).velocity;

	updateEntityRotation(entity, velocity);

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



void EnemyBehaviour::updateEntityRotation(Entity entity, myMath::Vector2D velocity) {
    auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
	auto& clockwise = ecsCoordinator.getComponent<EnemyComponent>(entity).isClockwise;

    // Only update rotation if there's meaningful movement
    const float minVelocityThreshold = 0.01f;
    float velocityMagnitude = static_cast<float>(std::sqrt(std::pow(velocity.GetX(), 2) + std::pow(velocity.GetY(), 2)));

    if (velocityMagnitude > minVelocityThreshold) {
        // Handle horizontal orientation (flipping)
        if (velocity.GetX() > minVelocityThreshold && !EnemyBehaviour::isFacingRight) {
            // Fish is moving right but facing left, so flip it
            transform.scale.SetX(std::abs(transform.scale.GetX())); // Make scale positive
			EnemyBehaviour::isFacingRight = true;
        }
        else if (velocity.GetX() < -minVelocityThreshold && EnemyBehaviour::isFacingRight) {
            // Fish is moving left but facing right, so flip it
            transform.scale.SetX(-std::abs(transform.scale.GetX())); // Make scale negative
			EnemyBehaviour::isFacingRight = false;
        }

        // Calculate proper rotation for up/down movement
        float rotationAngle = 0.0f;

        if (std::abs(velocity.GetY()) > minVelocityThreshold) {
			std::cout << velocity.GetY() << std::endl;
            float yDirection = 0.f;
            if (velocity.GetY() < 0.f) {
                if (clockwise) { yDirection = velocity.GetY(); }
                else { yDirection = -velocity.GetY(); }
			}
            else {
                if (clockwise) { yDirection = -velocity.GetY(); }
                else { yDirection = velocity.GetY(); }
            }

            float xComponent = isFacingRight ? std::abs(velocity.GetX()) : -std::abs(velocity.GetX());

            // Only calculate rotation if horizontal movement isn't dominant
            if (std::abs(velocity.GetY()) > 0.5f * std::abs(velocity.GetX())) {
                if (clockwise) {
                    if (velocity.GetY() > 0.f && isFacingRight) {
                        rotationAngle = static_cast<float>(std::atan2(yDirection, xComponent) * -(180.0 / 3.14159265358979323846));
                    }
                    else {
                        rotationAngle = static_cast<float>(std::atan2(yDirection, xComponent) * (180.0 / 3.14159265358979323846));
                    }
                }
                else {
                    if (velocity.GetY() < 0.f && isFacingRight) {
						rotationAngle = static_cast<float>(std::atan2(yDirection, xComponent) * -(180.0 / 3.14159265358979323846));
					}
                    else {
                        rotationAngle = static_cast<float>(std::atan2(yDirection, xComponent) * (180.0 / 3.14159265358979323846));
                    }
                }

                // Clamp the rotation to avoid extreme angles (-30� to 30�)
                rotationAngle = std::max(-90.f, std::min(rotationAngle, 90.f));
            }
        }

        // Apply rotation
		transform.orientation.SetX(rotationAngle);
		
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
    auto& forceManager = ecsCoordinator.getComponent<PhysicsComponent>(entity).forceManager;

    // Set the current waypoint target
    myMath::Vector2D target = currentWaypoints[currentWPIndex];

    if (moveHorizontal) {
        if (transform.position.GetX() >= target.GetX() - 1.f && transform.position.GetX() <= target.GetX() + 1.f) {
			transform.position.SetX(target.GetX());
			moveHorizontal = false;
            physics.accumulatedForce.SetX(0.f);
            physics.velocity.SetX(0.f);
		}
        else {
            if (transform.position.GetX() < target.GetX()) {
                forceManager.AddForce(entity, myMath::Vector2D(20.0f, 0.f));
            }
            else {
                forceManager.AddForce(entity, myMath::Vector2D(-20.0f, 0.f));
            }
        }
        // Physics calculations
        float invMass = physics.mass > 0.f ? 1.f / physics.mass : 0.f;
        physics.acceleration = physics.accumulatedForce * invMass;

        // Update velocity
        physics.velocity.SetX(physics.velocity.GetX() + physics.acceleration.GetX() * GLFWFunctions::delta_time);

        const float maxSpeed = 0.2f; // Example max speed
        if (physics.velocity.GetX() > maxSpeed) physics.velocity.SetX(maxSpeed);
        if (physics.velocity.GetX() < -maxSpeed) physics.velocity.SetX(-maxSpeed);

        // Apply velocity to position
        transform.position.SetX(transform.position.GetX() + physics.velocity.GetX());
    }
    else {
        if (transform.position.GetY() >= target.GetY() - 1.f && transform.position.GetY() <= target.GetY() + 1.f) {
            transform.position.SetY(target.GetY());
			//std::cout << transform.position.GetY() << ", " << target.GetY() << std::endl;
            moveHorizontal = true;
            physics.accumulatedForce.SetY(0.f);
            physics.velocity.SetY(0.f);
            currentWPIndex = (currentWPIndex + 1) % currentWaypoints.size();
        }
        else {
            if (transform.position.GetY() < target.GetY()) {
                forceManager.AddForce(entity, myMath::Vector2D(0.f, 20.0f));
            }
            else {
                forceManager.AddForce(entity, myMath::Vector2D(0.f, -20.0f));
            }
        }
        // Physics calculations
        float invMass = physics.mass > 0.f ? 1.f / physics.mass : 0.f;
        physics.acceleration = physics.accumulatedForce * invMass;
        // Update velocity
        physics.velocity.SetY(physics.velocity.GetY() + physics.acceleration.GetY() * GLFWFunctions::delta_time);
        const float maxSpeed = 0.2f; // Example max speed
        if (physics.velocity.GetY() > maxSpeed) physics.velocity.SetY(maxSpeed);
        if (physics.velocity.GetY() < -maxSpeed) physics.velocity.SetY(-maxSpeed);
        // Apply velocity to position
        transform.position.SetY(transform.position.GetY() + physics.velocity.GetY());
    }

}
