#pragma once
/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Ian Loi (ian.loi)
@team   :  MonkeHood
@course :  CSD2401
@file   :  vector2D.h
@brief  :  vector2D.h contains the class of vector2D and the declaration of all member and non-member functions
           for doing the basic operations of what a 2D vector should do. The declared functions will do the
           addition, subtraction between two 2D vectors, constructing a Vector2D class object, calculating
           normalized vector, length of vector, distance between two vectors and more.

*Ian Loi (ian.loi) : 
        - Declared functions of the vector2D class that will handle the different operations of a 2D
          vector.

File Contributions: Ian Loi (100%)

*//*_________________________________________________________________________________________________-*/


namespace myMath
{
    class Vector2D
    {
    public:
        Vector2D();
        Vector2D(float, float);
        Vector2D(Vector2D const&);

        float GetX() const;
        float GetY() const;

        void SetX(float);
        void SetY(float);

        Vector2D& operator=(Vector2D const&);
        Vector2D& operator=(Vector2D&);

        Vector2D& operator+=(Vector2D const&);
        Vector2D& operator-=(Vector2D const&);
        Vector2D& operator*=(float);
        Vector2D& operator/=(float);

        Vector2D operator-() const;

    private:
        float x;
        float y;
    };

    // non-member binary operation functions
    Vector2D operator+(Vector2D const&, Vector2D const&);
    Vector2D operator-(Vector2D const&, Vector2D const&);
    Vector2D operator*(Vector2D const&, float);
    Vector2D operator*(float, Vector2D const&);
    Vector2D operator/(Vector2D const&, float);

    // other non-member functions
    void NormalizeVector2D(Vector2D&, Vector2D const&);
    float LengthVector2D(Vector2D const&);
    float SquareLengthVector2D(Vector2D const&);
    float DistanceVector2D(Vector2D const&, Vector2D const&);
    float SquareDistanceVector2D(Vector2D const&, Vector2D const&);
    float DotProductVector2D(Vector2D const&, Vector2D const&);
    float CrossProductMagnitudeVector2D(Vector2D const&, Vector2D const&);
}
