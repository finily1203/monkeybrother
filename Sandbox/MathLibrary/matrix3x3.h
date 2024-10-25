#pragma once
/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Ian Loi (ian.loi)
@team   :  MonkeHood
@course :  CSD2401
@file   :  matrix3x3.h
@brief  :  matrix3x3.h contains the class of matrix3x3 and the declaration of all member and non-member functions
           The functions handle the basic operations like addition, subtraction of two 3 by 3 matrixes,
           and calculating translation matrix, rotation matrix, scaling matrix and many more.

*Ian Loi (ian.loi) : 
        - Declared functions of the matrix3x3 class that handles the basic operations of a 3 by 3 matrix
          like addition, subtraction between two matrixes, construction of a matrix3x3 class object and
          other operations.

File Contributions: Ian Loi (100%)

*//*_________________________________________________________________________________________________-*/

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
