/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Ian Loi (ian.loi)
@team   :  MonkeHood
@course :  CSD2401
@file   :  vector3D.cpp
@brief  :  vector3D.cpp handles vector3D operations like additions, subtractions of two 3D vectors,
           calculating the normalized vector, distance between two 3D vectors, etc.

*Ian Loi (ian.loi) :
        - Create functions of the vector3D class that will handle different operations of a 3D
          vector

File Contributions: Ian Loi (100%)

*//*_________________________________________________________________________________________________-*/


#include "vector3D.h"
#include <cmath>

namespace MyMath
{
    // constructors for vector3D class
    Vector3D::Vector3D() : x(0.f), y(0.f), z(0.f) {}
    Vector3D::Vector3D(float xPt, float yPt, float zPt) : x(xPt), y(yPt), z(zPt) {}
    Vector3D::Vector3D(Vector3D const& rhs) : x(rhs.x), y(rhs.y), z(rhs.z) {}

    // get x, y and z value functions
    float Vector3D::GetX() const { return x; }
    float Vector3D::GetY() const { return y; }
    float Vector3D::GetZ() const { return z; }

    // set x, y and z value functions
    void Vector3D::SetX(float xPt) { this->x = xPt; }
    void Vector3D::SetY(float yPt) { this->y = yPt; }
    void Vector3D::SetZ(float zPt) { this->z = zPt; }

    // copy assignment operators functions
    Vector3D& Vector3D::operator=(Vector3D const& rhs) { x = rhs.x; y = rhs.y; z = rhs.z; return *this; }
    Vector3D& Vector3D::operator=(Vector3D& rhs) { x = rhs.x; y = rhs.y; z = rhs.z; return *this; }

    // other assignment operators functions
    Vector3D& Vector3D::operator+=(Vector3D const& rhs) { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
    Vector3D& Vector3D::operator-=(Vector3D const& rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
    Vector3D& Vector3D::operator*=(float scalar) { x *= scalar; y *= scalar; z *= scalar; return *this; }
    Vector3D& Vector3D::operator/=(float scalar) { x /= scalar; y /= scalar; z /= scalar; return *this; }

    // negate operator function
    Vector3D Vector3D::operator-() const { return Vector3D(-x, -y, -z); }

    // binary operators function
    // overload addition operator
    Vector3D operator+(Vector3D const& lhs, Vector3D const& rhs)
    {
        return Vector3D(lhs.GetX() + rhs.GetX(), lhs.GetY() + rhs.GetY(), lhs.GetZ() + rhs.GetZ());
    }

    // overload subtraction operator
    Vector3D operator-(Vector3D const& lhs, Vector3D const& rhs)
    {
        return Vector3D(lhs.GetX() - rhs.GetX(), lhs.GetY() - rhs.GetY(), lhs.GetZ() - rhs.GetZ());
    }

    // overload multiplication operator with rhs being float scalar
    Vector3D operator*(Vector3D const& lhs, float rhs)
    {
        return Vector3D(lhs.GetX() * rhs, lhs.GetY() * rhs, lhs.GetZ() * rhs);
    }

    Vector3D operator*(float lhs, Vector3D const& rhs)
    {
        return Vector3D(lhs * rhs.GetX(), lhs * rhs.GetY(), lhs * rhs.GetZ());
    }

    Vector3D operator/(Vector3D const& lhs, float rhs)
    {
        return Vector3D(lhs.GetX() / rhs, lhs.GetY() / rhs, lhs.GetZ() / rhs);
    }

    // normalizing of the 3D vector
    void NormalizeVector3D(Vector3D& resultVector, Vector3D const& vector)
    {
        float vectorLength = LengthVector3D(vector);
        resultVector.SetX(vector.GetX() / vectorLength);
        resultVector.SetY(vector.GetY() / vectorLength);
        resultVector.SetZ(vector.GetZ() / vectorLength);
    }

    // finding length of vector
    float LengthVector3D(Vector3D const& vector)
    {
        // formula to find length of 3D vector
        return static_cast<float>(std::sqrt(std::pow(vector.GetX(), 2) + std::pow(vector.GetY(), 2) + std::pow(vector.GetZ(), 2)));
    }

    // finding sqaured length of vector
    float SquareLengthVector3D(Vector3D const& vector)
    {
        // formula to find squared length of 3D vector
        return static_cast<float>(std::pow(vector.GetX(), 2) + std::pow(vector.GetY(), 2) + std::pow(vector.GetZ(), 2));
    }

    // finding distance between two 3D vectors
    float DistanceVector3D(Vector3D const& vectorOne, Vector3D const& vectorTwo)
    {
        // formula to find distance between two 3D vectors
        return static_cast<float>(std::sqrt(std::pow(vectorOne.GetX() - vectorTwo.GetX(), 2) + std::pow(vectorOne.GetY() - vectorTwo.GetY(), 2) + std::pow(vectorOne.GetZ() - vectorTwo.GetZ(), 2)));
    }

    // finding squared distance between two 3D vectors
    float SquareDistanceVector3D(Vector3D const& vectorOne, Vector3D const& vectorTwo)
    {
        // formula to find squared distance between two 3D vectors
        return static_cast<float>(std::pow(vectorOne.GetX() - vectorTwo.GetX(), 2) + std::pow(vectorOne.GetY() - vectorTwo.GetY(), 2) + std::pow(vectorOne.GetZ() - vectorTwo.GetZ(), 2));
    }

    // finding dot product of two vectors
    float DotProductVector3D(Vector3D const& vectorOne, Vector3D const& vectorTwo)
    {
        // formula to find dot product of two 3D vectors
        return (vectorOne.GetX() * vectorTwo.GetX() + vectorOne.GetY() * vectorTwo.GetY() + vectorOne.GetZ() * vectorTwo.GetZ());
    }

    // finding the cross product magnitude of two vectors
    float CrossProductMagnitudeVector3D(Vector3D const& vectorOne, Vector3D const& vectorTwo)
    {
        // formula to find cross product magnitude of two 3D vectors
        float crossX = vectorOne.GetY() * vectorTwo.GetZ() - vectorOne.GetZ() * vectorTwo.GetY();
        float crossY = vectorOne.GetZ() * vectorTwo.GetX() - vectorOne.GetX() * vectorTwo.GetZ();
        float crossZ = vectorOne.GetX() * vectorTwo.GetY() - vectorOne.GetY() * vectorTwo.GetX();

        return static_cast<float>(std::sqrt(crossX * crossX + crossY * crossY + crossZ * crossZ));
    }
}
