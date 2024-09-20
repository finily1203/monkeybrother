#pragma once

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
