#include "vector2D.h"
#include <cmath>

namespace myMath
{
    // constructors are created here
    Vector2D::Vector2D() : x(0.f), y(0.f) {}                        // default constructor
    Vector2D::Vector2D(float xPt, float yPt) : x(xPt), y(yPt) {}    // constructor that takes in 2 parameters     
    Vector2D::Vector2D(Vector2D const& rhs) : x(rhs.x), y(rhs.y) {} // copy constructor

    // get x and y functions
    float Vector2D::GetX() const { return x; }
    float Vector2D::GetY() const { return y; }

    // set x and y functions
    void Vector2D::SetX(float xPt) { this->x = xPt; }
    void Vector2D::SetY(float yPt) { this->y = yPt; }

    // copy assisngment operators
    Vector2D& Vector2D::operator=(Vector2D const& rhs) { x = rhs.x; y = rhs.y; return *this; }  // copy assignment operator const
    Vector2D& Vector2D::operator=(Vector2D& rhs) { x = rhs.x; y = rhs.y; return *this; }        // copy assignment operator for copy-swap

    // other assignment operators
    Vector2D& Vector2D::operator+=(Vector2D const& rhs) { x += rhs.x; y += rhs.y; return *this; }
    Vector2D& Vector2D::operator-=(Vector2D const& rhs) { x -= rhs.x; y -= rhs.y; return *this; }
    Vector2D& Vector2D::operator*=(float scalar) { x *= scalar; y *= scalar; return *this; }
    Vector2D& Vector2D::operator/=(float scalar) { x /= scalar; y /= scalar; return *this; }

    // negate operator
    Vector2D Vector2D::operator-() const { return Vector2D(-x, -y); }

    // binary operators
    // overload addition operator
    Vector2D operator+(Vector2D const& lhs, Vector2D const& rhs)
    {
        return Vector2D(lhs.GetX() + rhs.GetX(), lhs.GetY() + rhs.GetY());
    }

    // overload subtraction operator
    Vector2D operator-(Vector2D const& lhs, Vector2D const& rhs)
    {
        return Vector2D(lhs.GetX() - rhs.GetX(), lhs.GetY() - rhs.GetY());
    }

    // overload multiplication operator with rhs being the scalar
    Vector2D operator*(Vector2D const& lhs, float rhs)
    {
        return Vector2D(lhs.GetX() * rhs, lhs.GetY() * rhs);
    }

    // overload multiplication operator with lhs being the scalar
    Vector2D operator*(float lhs, Vector2D const& rhs)
    {
        return Vector2D(lhs * rhs.GetX(), lhs * rhs.GetY());
    }

    // overload division operator
    Vector2D operator/(Vector2D const& lhs, float rhs)
    {
        return Vector2D(lhs.GetX() / rhs, lhs.GetY() / rhs);
    }

    // normalizing vector function
    void NormalizeVector2D(Vector2D& resultVector, Vector2D const& vector)
    {
        float vectorLength = LengthVector2D(vector);
        resultVector.SetX(vector.GetX() / vectorLength);
        resultVector.SetY(vector.GetY() / vectorLength);
    }

    // finding length of vector function
    float LengthVector2D(Vector2D const& vector)
    {
        // formula to find the length of a vector
        return static_cast<float>(std::sqrt(std::pow(vector.GetX(), 2) + std::pow(vector.GetY(), 2)));
    }

    // finding squared length of vector function
    float SquareLengthVector2D(Vector2D const& vector)
    {
        // formula to find the squared length of a vector
        return static_cast<float>(std::pow(vector.GetX(), 2) + std::pow(vector.GetY(), 2));
    }

    // finding distance between two vectors
    float DistanceVector2D(Vector2D const& vectorOne, Vector2D const& vectorTwo)
    {
        return static_cast<float>(std::sqrt(std::pow(vectorOne.GetX() - vectorTwo.GetX(), 2) + std::pow(vectorOne.GetY() - vectorTwo.GetY(), 2)));
    }

    // finding squared distance between two vectors
    float SquareDistanceVector2D(Vector2D const& vectorOne, Vector2D const& vectorTwo)
    {
        return static_cast<float>(std::pow(vectorOne.GetX() - vectorTwo.GetX(), 2) + std::pow(vectorTwo.GetY() - vectorTwo.GetY(), 2));
    }

    // finding dot product of two vectors
    float DotProductVector2D(Vector2D const& vectorOne, Vector2D const& vectorTwo)
    {
        return (vectorOne.GetX() * vectorTwo.GetX() + vectorOne.GetY() * vectorTwo.GetY());
    }

    // finding cross product magnitude of two vectors
    float CrossProductMagnitudeVector2D(Vector2D const& vectorOne, Vector2D const& vectorTwo)
    {
        return ((vectorOne.GetX() * vectorTwo.GetX()) - (vectorOne.GetY() * vectorTwo.GetY()));
    }
}
