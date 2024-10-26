#include "../MathLibrary/vector2D.h"

#ifndef PHYSICS_H
#define PHYSICS_H

namespace PhysicsFramework {

    class Physics {
    public:
        Physics();

        myMath::Vector2D GetVelocity() const { return velocity; }
        bool GetAlrJumped() const { return alrJumped; }

        void SetVelocity(myMath::Vector2D newVelocity) { velocity = newVelocity; }
        void SetAlrJumped(bool newAlrJumped) { alrJumped = newAlrJumped; }


        void ApplyGravity(float slopeAngle);
        void Jump();

    private:
        myMath::Vector2D velocity;
        float gravity, jumpForce;
        bool alrJumped;
    };

    extern Physics* physics;
}
#endif // PHYSICS_H

