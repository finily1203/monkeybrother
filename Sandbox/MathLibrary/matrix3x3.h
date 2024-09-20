#pragma once

#include "vector2D.h"


namespace myMath
{
    class Matrix3x3
    {
    public:
        Matrix3x3();
        Matrix3x3(const float*);
        Matrix3x3(float, float, float,
            float, float, float,
            float, float, float);
        Matrix3x3(Matrix3x3 const&);

        float GetMatrixValue(int, int) const;
        void SetMatrixValue(int, int, float);

        Matrix3x3& operator=(Matrix3x3 const&);
        Matrix3x3& operator=(Matrix3x3&);

        Matrix3x3& operator+=(Matrix3x3 const&);
        Matrix3x3& operator-=(Matrix3x3 const&);
        Matrix3x3& operator*=(Matrix3x3 const&);

    private:
        float matrix[3][3];
    };

    Matrix3x3 operator+(Matrix3x3 const&, Matrix3x3 const&);
    Matrix3x3 operator-(Matrix3x3 const&, Matrix3x3 const&);
    Matrix3x3 operator*(Matrix3x3 const&, Matrix3x3 const&);
    Vector2D operator*(Matrix3x3 const&, Vector2D const&);
    Matrix3x3 operator*(Matrix3x3 const&, float);
    Matrix3x3 operator*(float, Matrix3x3 const&);

    void IdentityMatrix3x3(Matrix3x3&);
    void TranslateMatrix3x3(Matrix3x3&, float, float);
    void ScaleMatrix3x3(Matrix3x3&, float, float);
    void RotateRadMatrix3x3(Matrix3x3&, float);
    void RotateDegMatrix3x3(Matrix3x3&, float);
    void TransposeMatrix3x3(Matrix3x3&, Matrix3x3 const&);
    void InverseMatrix3x3(Matrix3x3*, float*, Matrix3x3 const&);
}
