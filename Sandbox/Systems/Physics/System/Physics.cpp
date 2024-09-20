#include "Physics.h"
#include "../GLFWFunctions/GlfwFunctions.h"
#include <cmath>

namespace PhysicsFramework {

	Physics* physics = new Physics();

	Physics::Physics() : velocity{ 0, 0 }, gravity{ -30.f }, jumpForce{ 11.f }, alrJumped{ false } {}

	void Physics::ApplyGravity(float slopeAngle) {
		float gravityX = gravity * sinf(slopeAngle);
		float gravityY = gravity * cosf(slopeAngle);

		SetVelocity({ GetVelocity().GetX() + gravityX * GLFWFunctions::delta_time, GetVelocity().GetY() + gravity * GLFWFunctions::delta_time });

	}

	void Physics::Jump() {
		if (!GetAlrJumped()) {
			SetVelocity({ 0, jumpForce });
			SetAlrJumped(true); // Set the jump flag; player can't jump again until they land (no double jump)
		}
	}
}


// FOR PLAYER MOVEMENT (IMPLEMENTATION OF PHYSICS)

//void Player::PlayerMovement() {
//
//    physics->ApplyGravity(0);
//
//    SetCoordinate({ GetCoordinate().x + physics->GetVelocity().x, GetCoordinate().y + physics->GetVelocity().y });
//
//    f32 rotationSpeed = 0.05f; // Arbitrary speed factor for rotation
//    rotationAngle += rotationSpeed * physics->GetVelocity().x;  // Increment angle based on x velocity
//
//    // Wrap angle to avoid overflow
//    if (rotationAngle > 360.0f)
//        rotationAngle -= 360.0f;
//    else if (rotationAngle < 0.0f)
//        rotationAngle += 360.0f;
//
//    if (GetCoordinate().y <= 0) {
//        SetCoordinate({ GetCoordinate().x , 0 });
//        physics->SetVelocity({ 0, 0 });
//        physics->SetAlrJumped(false); // Reset the jump flag
//    }
//
//    if (AEInputCheckCurr(AEVK_SPACE)) {
//        physics->Jump();
//    }
//
//    if ((AEInputCheckCurr(AEVK_A) && AEInputCheckCurr(AEVK_D))) {
//        physics->SetVelocity({ 0, physics->GetVelocity().y });
//    }
//
//    else if ((AEInputCheckCurr(AEVK_A))) {
//        SetCoordinate({ GetCoordinate().x - speed, GetCoordinate().y });
//    }
//
//    else if ((AEInputCheckCurr(AEVK_D))) {
//        SetCoordinate({ GetCoordinate().x + speed, GetCoordinate().y });
//    }
//}
