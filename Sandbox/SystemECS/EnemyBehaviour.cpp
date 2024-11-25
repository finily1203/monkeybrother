#include "EnemyBehaviour.h"
#include "GlobalCoordinator.h"
#include "PhyColliSystemECS.h"

#include "EnemyBehaviour.h"  
#include "GlobalCoordinator.h"  
#include "PhyColliSystemECS.h"  

EnemyBehaviour::EnemyBehaviour() {
	currentState = PATROL;

	//For now all enemies have same way point
	waypoints.push_back(myMath::Vector2D(400, 200));
	waypoints.push_back(myMath::Vector2D(-400, 200));
	waypoints.push_back(myMath::Vector2D(-400, -200));
	waypoints.push_back(myMath::Vector2D(400, -200));

}

void EnemyBehaviour::switchState(STATE newState) {  
   currentState = newState;  
}

void EnemyBehaviour::update(Entity entity) {
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
	auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
	auto& physics = ecsCoordinator.getComponent<PhysicsComponent>(entity);
	auto& waypoints = getWaypoints();
	int& currentWaypointIndex = getCurrentWaypointIndex();

	//set waypoint target
	myMath::Vector2D target = waypoints[currentWaypointIndex];
	physics.force.SetDirection(target - transform.position);

	//check if enemy is close to waypoint
	if (physics.force.GetMagnitude() < 10.0f) {
		currentWaypointIndex++;
		if (currentWaypointIndex >= waypoints.size()) {
			currentWaypointIndex = 0;
		}
	}
}



//void EnemyBehaviour::update(Entity entity) {
//	auto PhysicsSystemRef = ecsCoordinator.getSpecificSystem<PhysicsSystemECS>();
//	auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
//
//	myMath::Vector2D velocity = ecsCoordinator.getComponent<PhysicsComponent>(entity).velocity;
//
//	if (GLFWFunctions::keyState[Key::LEFT]) {
//		transform.orientation.SetY(transform.orientation.GetY() + (180.f * GLFWFunctions::delta_time));
//	}
//	else if (GLFWFunctions::keyState[Key::RIGHT]) {
//		transform.orientation.SetY(transform.orientation.GetY() - (180.0f * GLFWFunctions::delta_time));
//	}
//
//	if (GLFWFunctions::keyState[Key::UP]) {
//		if (transform.scale.GetX() < 500.0f && transform.scale.GetY() < 500.0f) {
//			transform.scale.SetX(transform.scale.GetX() + 53.4f * GLFWFunctions::delta_time);
//			transform.scale.SetY(transform.scale.GetY() + 30.0f * GLFWFunctions::delta_time);
//		}
//	}
//	else if (GLFWFunctions::keyState[Key::DOWN]) {
//		if (transform.scale.GetX() > 100.0f && transform.scale.GetY() > 100.0f) {
//			transform.scale.SetX(transform.scale.GetX() - 53.4f * GLFWFunctions::delta_time);
//			transform.scale.SetY(transform.scale.GetY() - 30.0f * GLFWFunctions::delta_time);
//		}
//	}
//
//	myMath::Vector2D& ePos = ecsCoordinator.getComponent<TransformComponent>(entity).position;
//	myMath::Vector2D eAcceleration = ecsCoordinator.getComponent<PhysicsComponent>(entity).acceleration;
//	//myMath::Vector2D eForce		   = ecsCoordinator.getComponent<PhysicsComponent>(entity).force;
//	myMath::Vector2D& eVel = ecsCoordinator.getComponent<PhysicsComponent>(entity).velocity;
//	myMath::Vector2D& eAccForce = ecsCoordinator.getComponent<PhysicsComponent>(entity).accumulatedForce;
//	float eMass = ecsCoordinator.getComponent<PhysicsComponent>(entity).mass;
//	//float eGravityScale			   = ecsCoordinator.getComponent<PhysicsComponent>(entity).gravityScale;
//	float eMagnitude = ecsCoordinator.getComponent<PhysicsComponent>(entity).force.GetMagnitude();
//
//	if (GLFWFunctions::keyState[Key::I]) {
//		PhysicsSystemRef->getForceManager().AddForce(entity, myMath::Vector2D(0.0f, eMagnitude));
//	}
//	else if (GLFWFunctions::keyState[Key::K]) {
//		PhysicsSystemRef->getForceManager().AddForce(entity, myMath::Vector2D(0.0f, -eMagnitude));
//	}
//	else if (GLFWFunctions::keyState[Key::J]) {
//		PhysicsSystemRef->getForceManager().AddForce(entity, myMath::Vector2D(-eMagnitude, 0.0f));
//	}
//	else if (GLFWFunctions::keyState[Key::L]) {
//		PhysicsSystemRef->getForceManager().AddForce(entity, myMath::Vector2D(eMagnitude, 0.0f));
//	}
//	else {
//
//		//Apply friction to gradually slow down
//		if (eAccForce.GetX() > 0) {
//			PhysicsSystemRef->getForceManager().AddForce(entity, myMath::Vector2D(-eMagnitude, 0.0f));
//		}
//		else if (eAccForce.GetX() < 0) {
//			PhysicsSystemRef->getForceManager().AddForce(entity, myMath::Vector2D(eMagnitude, 0.0f));
//		}
//		else if (eAccForce.GetY() > 0) {
//			PhysicsSystemRef->getForceManager().AddForce(entity, myMath::Vector2D(0.0f, -eMagnitude));
//		}
//		else if (eAccForce.GetY() < 0) {
//			PhysicsSystemRef->getForceManager().AddForce(entity, myMath::Vector2D(0.0f, eMagnitude));
//		}
//
//		if (std::abs(eAccForce.GetX()) < 0.01f) { // threshold
//			eVel.SetX(0.f);
//			eAccForce.SetX(0.f);
//		}
//		if (std::abs(eAccForce.GetY()) < 0.01f) { // threshold
//			eVel.SetY(0.f);
//			eAccForce.SetY(0.f);
//		}
//	}
//
//	float invMass = eMass > 0.f ? 1.f / eMass : 0.f;
//	eAcceleration = eAccForce * invMass;
//	const float maxSpeed = 0.6f;
//
//	// Clamp speed without interfering with velocity
//	if (eVel.GetX() > maxSpeed) {
//		eAcceleration.SetX(0);
//	}
//	else if (eVel.GetX() < -maxSpeed) {
//		eAcceleration.SetX(0);
//	}
//
//	eVel.SetX(eVel.GetX() + eAcceleration.GetX() * GLFWFunctions::delta_time);
//	eVel.SetY(eVel.GetY() + eAcceleration.GetY() * GLFWFunctions::delta_time);
//
//	eVel.SetX(eVel.GetX() * 0.9f);
//	eVel.SetY(eVel.GetY() * 0.9f);
//
//	ePos.SetX(ePos.GetX() + eVel.GetX());
//	ePos.SetY(ePos.GetY() + eVel.GetY());
//}