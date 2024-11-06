#pragma once
#include "vector2D.h"
#define M_PI   3.14159265358979323846264338327950288f

class ForceManager {
public:
	void ApplyForce(Entity entity, myMath::Vector2D force);
	myMath::Vector2D clampVelocity(myMath::Vector2D velocity, float maxSpeed);

};

extern ForceManager forceManager;

struct RigidBodyComponent
{
	float mass;
	myMath::Vector2D gravityScale;
	float jump;
	float dampening;
	myMath::Vector2D velocity;
	myMath::Vector2D acceleration;
	myMath::Vector2D force;
	myMath::Vector2D accumulatedForce;

	RigidBodyComponent() : mass(1.0f), gravityScale(0.0f, 0.0f), jump(0.0f), dampening(0.0f), 
						   velocity(0.0f, 0.0f), acceleration(0.0f, 0.0f), force(0.0f, 0.0f), 
						   accumulatedForce(0.0f, 0.0f) {}
};

//class Force {
//private:
//    float x;
//    float y;
//    float magnitude;
//    float direction;
//
//    void updateMagnitudeAndDirection() {
//        magnitude = std::sqrt(x * x + y * y);
//        direction = std::atan2(y, x);
//    }
//
//    void updateComponents() {
//        x = magnitude * std::cos(direction * (M_PI/2));
//        y = magnitude * std::sin(direction * (M_PI/2));
//    }
//
//public:
//    // Constructors
//    Force() : x(0.0f), y(0.0f), magnitude(0.0f), direction(0.0f) {}
//
//    Force(float x_, float y_) : x(x_), y(y_) {
//        updateMagnitudeAndDirection();
//    }
//
//    Force(float magnitude_, float direction_, bool isPolar) : magnitude(magnitude_), direction(direction_) {
//        if (isPolar) {
//            updateComponents();
//        }
//        else {
//            x = magnitude_;
//            y = direction_;
//            updateMagnitudeAndDirection();
//        }
//    }
//
//    // Getters
//    float getX() const { return x; }
//    float getY() const { return y; }
//    float getMagnitude() const { return magnitude; }
//    float getDirection() const { return direction; }
//
//    // Setters
//    void setX(float x_) {
//        x = x_;
//        updateMagnitudeAndDirection();
//    }
//
//    void setY(float y_) {
//        y = y_;
//        updateMagnitudeAndDirection();
//    }
//
//    void setMagnitude(float magnitude_) {
//        magnitude = magnitude_;
//        updateComponents();
//    }
//
//    void setDirection(float direction_) {
//        direction = direction_;
//        updateComponents();
//    }
//
//    // Basic operations
//    Force& operator+=(const Force& other) {
//        x += other.x;
//        y += other.y;
//        updateMagnitudeAndDirection();
//        return *this;
//    }
//
//    Force& operator-=(const Force& other) {
//        x -= other.x;
//        y -= other.y;
//        updateMagnitudeAndDirection();
//        return *this;
//    }
//
//    Force& operator*=(float scalar) {
//        x *= scalar;
//        y *= scalar;
//        magnitude *= scalar;
//        return *this;
//    }
//
//    Force& operator/=(float scalar) {
//        if (scalar != 0.0f) {
//            x /= scalar;
//            y /= scalar;
//            magnitude /= scalar;
//        }
//        return *this;
//    }
//
//    // Static force creators
//    static Force createGravity(float mass, float gravityConstant = 9.81f) {
//        return Force(0.0f, -mass * gravityConstant);
//    }
//
//    static Force createDrag(const Force& velocity, float dragCoefficient) {
//        return Force(-velocity.x * dragCoefficient, -velocity.y * dragCoefficient);
//    }
//
//    static Force createSpring(const Force& displacement, float springConstant) {
//        return Force(-displacement.x * springConstant, -displacement.y * springConstant);
//    }
//
//    // Utility functions
//    void normalize() {
//        if (magnitude != 0.0f) {
//            x /= magnitude;
//            y /= magnitude;
//            magnitude = 1.0f;
//        }
//    }
//
//    Force getNormalized() const {
//        Force result = *this;
//        result.normalize();
//        return result;
//    }
//
//    float dot(const Force& other) const {
//        return x * other.x + y * other.y;
//    }
//
//    void zero() {
//        x = y = magnitude = direction = 0.0f;
//    }
//
//    bool isZero() const {
//        return magnitude < 0.000001f;
//    }
//};
//
//// Operator overloads
//inline Force operator+(Force lhs, const Force& rhs) {
//    lhs += rhs;
//    return lhs;
//}
//
//inline Force operator-(Force lhs, const Force& rhs) {
//    lhs -= rhs;
//    return lhs;
//}
//
//inline Force operator*(Force lhs, float rhs) {
//    lhs *= rhs;
//    return lhs;
//}
//
//inline Force operator*(float lhs, Force rhs) {
//    rhs *= lhs;
//    return rhs;
//}
//
//inline Force operator/(Force lhs, float rhs) {
//    lhs /= rhs;
//    return lhs;
//}
