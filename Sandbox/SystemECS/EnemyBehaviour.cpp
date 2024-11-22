#include "EnemyBehaviour.h"
#include "GlobalCoordinator.h"
#include "PhyColliSystemECS.h"

void EnemyBehaviour::update(Entity entity, float dt) {
	auto PhysicsSystemRef = ecsCoordinator.getSpecificSystem<PhysicsSystemECS>();
	auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);

	myMath::Vector2D velocity = ecsCoordinator.getComponent<PhysicsComponent>(entity).velocity;

	if (GLFWFunctions::keyState[Key::LEFT]) {
		transform.orientation.SetY(transform.orientation.GetY() + (180.f * dt));
	}
	else if (GLFWFunctions::keyState[Key::RIGHT]) {
		transform.orientation.SetY(transform.orientation.GetY() - (180.0f * dt));
	}

	if (GLFWFunctions::keyState[Key::UP]) {
		if (transform.scale.GetX() < 500.0f && transform.scale.GetY() < 500.0f) {
			transform.scale.SetX(transform.scale.GetX() + 53.4f * dt);
			transform.scale.SetY(transform.scale.GetY() + 30.0f * dt);
		}
	}
	else if (GLFWFunctions::keyState[Key::DOWN]) {
		if (transform.scale.GetX() > 100.0f && transform.scale.GetY() > 100.0f) {
			transform.scale.SetX(transform.scale.GetX() - 53.4f * dt);
			transform.scale.SetY(transform.scale.GetY() - 30.0f * dt);
		}
	}

	myMath::Vector2D& ePos = ecsCoordinator.getComponent<TransformComponent>(entity).position;
	myMath::Vector2D eAcceleration = ecsCoordinator.getComponent<PhysicsComponent>(entity).acceleration;
	//myMath::Vector2D eForce		   = ecsCoordinator.getComponent<PhysicsComponent>(entity).force;
	myMath::Vector2D& eVel = ecsCoordinator.getComponent<PhysicsComponent>(entity).velocity;
	myMath::Vector2D& eAccForce = ecsCoordinator.getComponent<PhysicsComponent>(entity).accumulatedForce;
	float eMass = ecsCoordinator.getComponent<PhysicsComponent>(entity).mass;
	//float eGravityScale			   = ecsCoordinator.getComponent<PhysicsComponent>(entity).gravityScale;
	float eMagnitude = ecsCoordinator.getComponent<PhysicsComponent>(entity).force.GetMagnitude();

	if (GLFWFunctions::keyState[Key::I]) {
		PhysicsSystemRef->getForceManager().AddForce(entity, myMath::Vector2D(0.0f, eMagnitude));
	}
	else if (GLFWFunctions::keyState[Key::K]) {
		PhysicsSystemRef->getForceManager().AddForce(entity, myMath::Vector2D(0.0f, -eMagnitude));
	}
	else if (GLFWFunctions::keyState[Key::J]) {
		PhysicsSystemRef->getForceManager().AddForce(entity, myMath::Vector2D(-eMagnitude, 0.0f));
	}
	else if (GLFWFunctions::keyState[Key::L]) {
		PhysicsSystemRef->getForceManager().AddForce(entity, myMath::Vector2D(eMagnitude, 0.0f));
	}
	else {

		//Apply friction to gradually slow down
		if (eAccForce.GetX() > 0) {
			PhysicsSystemRef->getForceManager().AddForce(entity, myMath::Vector2D(-eMagnitude, 0.0f));
		}
		else if (eAccForce.GetX() < 0) {
			PhysicsSystemRef->getForceManager().AddForce(entity, myMath::Vector2D(eMagnitude, 0.0f));
		}
		else if (eAccForce.GetY() > 0) {
			PhysicsSystemRef->getForceManager().AddForce(entity, myMath::Vector2D(0.0f, -eMagnitude));
		}
		else if (eAccForce.GetY() < 0) {
			PhysicsSystemRef->getForceManager().AddForce(entity, myMath::Vector2D(0.0f, eMagnitude));
		}

		if (std::abs(eAccForce.GetX()) < 0.01f) { // threshold
			eVel.SetX(0.f);
			eAccForce.SetX(0.f);
		}
		if (std::abs(eAccForce.GetY()) < 0.01f) { // threshold
			eVel.SetY(0.f);
			eAccForce.SetY(0.f);
		}
	}

	float invMass = eMass > 0.f ? 1.f / eMass : 0.f;
	eAcceleration = eAccForce * invMass;
	const float maxSpeed = 0.6f;

	// Clamp speed without interfering with velocity
	if (eVel.GetX() > maxSpeed) {
		eAcceleration.SetX(0);
	}
	else if (eVel.GetX() < -maxSpeed) {
		eAcceleration.SetX(0);
	}

	eVel.SetX(eVel.GetX() + eAcceleration.GetX() * dt);
	eVel.SetY(eVel.GetY() + eAcceleration.GetY() * dt);

	eVel.SetX(eVel.GetX() * 0.9f);
	eVel.SetY(eVel.GetY() * 0.9f);

	ePos.SetX(ePos.GetX() + eVel.GetX());
	ePos.SetY(ePos.GetY() + eVel.GetY());
}