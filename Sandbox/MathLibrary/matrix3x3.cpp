/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Ian Loi (ian.loi)
@team   :  MonkeHood
@course :  CSD2401
@file   :  matrix3x3.cpp
@brief  :  matrix3x3.cpp handles matrix3x3 operations like additions, subtractions, etc.

*Ian Loi (ian.loi) :
        - Create functions of the matrix3x3 class that handles different operations of a 3 by 3
          matrix like addition, subtraction between two matrixes, constructing a matrix3x3 class
          object and calculating scaling matrix, rotation matrix, inverse matrix and many more.

File Contributions: Ian Loi (100%)

*//*_________________________________________________________________________________________________-*/


#include "matrix3x3.h"
#include <cmath>


constexpr float PI{ 3.1415926f };

namespace myMath
{
    // Matrix3x3 constructor
    Matrix3x3::Matrix3x3()
    {
        // initialize the matrix array to all 0
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                matrix[i][j] = 0.f;
            }
        }
    }

    // single argument constructor for Matrix3x3
    Matrix3x3::Matrix3x3(const float* pArr)
    {
        // initialize a matrix3x3 array to the values of the pointer to an
        // array
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                matrix[i][j] = pArr[i * 3 + j];
            }
        }
    }

    // initialize a matrix3x3 array with all the arguments 
    Matrix3x3::Matrix3x3(float m00, float m01, float m02,
        float m10, float m11, float m12,
        float m20, float m21, float m22) : matrix{ {m00, m01, m02},
                                                  {m10, m11, m12},
                                                  {m20, m21, m22} }
    {}

    // copy constructor 
    Matrix3x3::Matrix3x3(Matrix3x3 const& rhs)
    {
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                matrix[i][j] = rhs.matrix[i][j];
            }
        }
    }

    // retrieve the matrix value based on the row and column index 
    float Matrix3x3::GetMatrixValue(int row, int col) const
    {
        return matrix[row][col];
    }

    // set the matrix value based on the row and column index
    void Matrix3x3::SetMatrixValue(int row, int col, float value)
    {
        if (row >= 0 && row < 3 && col >= 0 && col < 3)
        {
            matrix[row][col] = value;
        }
    }

    // copy assignment operator
    Matrix3x3& Matrix3x3::operator=(Matrix3x3 const& rhs)
    {
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                matrix[i][j] = rhs.matrix[i][j];
            }
        }

        return *this;
    }

    // copy assignment operator
    Matrix3x3& Matrix3x3::operator=(Matrix3x3& rhs)
    {
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                matrix[i][j] = rhs.matrix[i][j];
            }
        }

        return *this;
    }

    // addition assignment operator
    Matrix3x3& Matrix3x3::operator+=(Matrix3x3 const& rhs)
    {
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                matrix[i][j] += rhs.matrix[i][j];
            }
        }

        return *this;
    }

    // subtraction assignment operator
    Matrix3x3& Matrix3x3::operator-=(Matrix3x3 const& rhs)
    {
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                matrix[i][j] -= rhs.matrix[i][j];
            }
        }

        return *this;
    }

    // multiplication assignment operator
    Matrix3x3& Matrix3x3::operator*=(Matrix3x3 const& rhs)
    {
        Matrix3x3 tmp = *this;

        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                matrix[i][j] = tmp.matrix[i][0] * rhs.matrix[0][j] +
                    tmp.matrix[i][1] * rhs.matrix[1][j] +
                    tmp.matrix[i][2] * rhs.matrix[2][j];
            }
        }

        return *this;
    }

    // conversion function that convert my Matrix3x3 to a glm::mat3
    glm::mat3 Matrix3x3::ConvertToGLMMat3(Matrix3x3 const& rhs)
    {
        glm::mat3 glmMatrix{};

        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                glmMatrix[i][j] = rhs.matrix[i][j];
            }
        }

        return glmMatrix;
    }

    // conversion function that convert glm::mat3 to my Matrix3x3
    Matrix3x3 Matrix3x3::ConvertToMatrix3x3(glm::mat3 const& rhs)
    {
        Matrix3x3 myMatrix;

        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                myMatrix.SetMatrixValue(i, j, rhs[i][j]);
            }
        }

        return myMatrix;
    }

    // addition operator
    Matrix3x3 operator+(Matrix3x3 const& lhs, Matrix3x3 const& rhs)
    {
        Matrix3x3 result;

        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                result.SetMatrixValue(i, j, lhs.GetMatrixValue(i, j) + rhs.GetMatrixValue(i, j));
            }
        }

        return result;
    }

    // subtraction operator
    Matrix3x3 operator-(Matrix3x3 const& lhs, Matrix3x3 const& rhs)
    {
        Matrix3x3 result;

        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                result.SetMatrixValue(i, j, lhs.GetMatrixValue(i, j) - rhs.GetMatrixValue(i, j));
            }
        }

        return result;
    }

    // multiplication operator. multiply two Matrix3x3 object
    Matrix3x3 operator*(Matrix3x3 const& lhs, Matrix3x3 const& rhs)
    {
        Matrix3x3 result;

        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                result.SetMatrixValue(i, j, lhs.GetMatrixValue(i, 0) * rhs.GetMatrixValue(0, j) +
                    lhs.GetMatrixValue(i, 1) * rhs.GetMatrixValue(1, j) +
                    lhs.GetMatrixValue(i, 2) * rhs.GetMatrixValue(2, j));
            }
        }

        return result;
    }

    // multiplication operator, multiply one Matrix3x3 and one Vector2D
    Vector2D operator*(Matrix3x3 const& lhs, Vector2D const& rhs)
    {
        float x = lhs.GetMatrixValue(0, 0) * rhs.GetX() + lhs.GetMatrixValue(0, 1) * rhs.GetY() + lhs.GetMatrixValue(0, 2) * 1.f;
        float y = lhs.GetMatrixValue(1, 0) * rhs.GetX() + lhs.GetMatrixValue(1, 1) * rhs.GetY() + lhs.GetMatrixValue(1, 2) * 1.f;

        return Vector2D(x, y);
    }

    // multiplication operator, multiply a Matrix3x3 object with a scalar
    Matrix3x3 operator*(Matrix3x3 const& lhs, float rhs)
    {
        Matrix3x3 result;

        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                result.SetMatrixValue(i, j, lhs.GetMatrixValue(i, j) * rhs);
            }
        }

        return result;
    }

    // multiplication operator, multiply a Matrix3x3 object with a scalar 
    Matrix3x3 operator*(float lhs, Matrix3x3 const& rhs)
    {
        Matrix3x3 result;

        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                result.SetMatrixValue(i, j, lhs * rhs.GetMatrixValue(i, j));
            }
        }

        return result;
    }

    // making result to be an identity matrix
    void IdentityMatrix3x3(Matrix3x3& result)
    {
        result = Matrix3x3(1.f, 0.f, 0.f,
            0.f, 1.f, 0.f,
            0.f, 0.f, 1.f);
    }

    // result will be set to a translation matrix
    void TranslateMatrix3x3(Matrix3x3& result, float x, float y)
    {
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                result.SetMatrixValue(i, j, (i == j) ? 1.f : 0.f);
            }
        }

        result.SetMatrixValue(0, 2, x);
        result.SetMatrixValue(1, 2, y);
    }

    // result will be set to a scaling matrix
    void ScaleMatrix3x3(Matrix3x3& result, float x, float y)
    {
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                result.SetMatrixValue(i, j, (i == j) ? 1.f : 0.f);
            }
        }

        result.SetMatrixValue(0, 0, x);
        result.SetMatrixValue(1, 1, y);
    }

    // result will be set to a rotation matrix by radian
    void RotateRadMatrix3x3(Matrix3x3& result, float angle)
    {
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                result.SetMatrixValue(i, j, (i == j) ? 1.f : 0.f);
            }
        }

        result.SetMatrixValue(0, 0, std::cos(angle));
        result.SetMatrixValue(0, 1, -std::sin(angle));
        result.SetMatrixValue(1, 0, std::sin(angle));
        result.SetMatrixValue(1, 1, std::cos(angle));
    }

    // result will be set to a rotation matrix by degree
    void RotateDegMatrix3x3(Matrix3x3& result, float angle)
    {
        float angleDeg = angle * (PI / 180.f);

        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                result.SetMatrixValue(i, j, (i == j) ? 1.f : 0.f);
            }
        }

        result.SetMatrixValue(0, 0, std::cos(angleDeg));
        result.SetMatrixValue(0, 1, -std::sin(angleDeg));
        result.SetMatrixValue(1, 0, std::sin(angleDeg));
        result.SetMatrixValue(1, 1, std::cos(angleDeg));
    }

    // set result to be a transpose matrix
    void TransposeMatrix3x3(Matrix3x3& result, Matrix3x3 const& matrix)
    {
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                result.SetMatrixValue(i, j, matrix.GetMatrixValue(j, i));
            }
        }
    }

    // set result to be an inverse matrix
    void InverseMatrix3x3(Matrix3x3* result, float* determinant, Matrix3x3 const& matrix)
    {
        Matrix3x3 cofactorMatrix;

        cofactorMatrix.SetMatrixValue(0, 0, matrix.GetMatrixValue(1, 1) * matrix.GetMatrixValue(2, 2) -
            matrix.GetMatrixValue(1, 2) * matrix.GetMatrixValue(2, 1));

        cofactorMatrix.SetMatrixValue(0, 1, -(matrix.GetMatrixValue(1, 0) * matrix.GetMatrixValue(2, 2) -
            matrix.GetMatrixValue(1, 2) * matrix.GetMatrixValue(2, 0)));

        cofactorMatrix.SetMatrixValue(0, 2, matrix.GetMatrixValue(1, 0) * matrix.GetMatrixValue(2, 1) -
            matrix.GetMatrixValue(1, 1) * matrix.GetMatrixValue(2, 0));

        cofactorMatrix.SetMatrixValue(1, 0, -(matrix.GetMatrixValue(0, 1) * matrix.GetMatrixValue(2, 2) -
            matrix.GetMatrixValue(0, 2) * matrix.GetMatrixValue(2, 1)));

        cofactorMatrix.SetMatrixValue(1, 1, matrix.GetMatrixValue(0, 0) * matrix.GetMatrixValue(2, 2) -
            matrix.GetMatrixValue(0, 2) * matrix.GetMatrixValue(2, 0));

        cofactorMatrix.SetMatrixValue(1, 2, -(matrix.GetMatrixValue(0, 0) * matrix.GetMatrixValue(2, 1) -
            matrix.GetMatrixValue(0, 1) * matrix.GetMatrixValue(2, 0)));

        cofactorMatrix.SetMatrixValue(2, 0, matrix.GetMatrixValue(0, 1) * matrix.GetMatrixValue(1, 2) -
            matrix.GetMatrixValue(0, 2) * matrix.GetMatrixValue(1, 1));

        cofactorMatrix.SetMatrixValue(2, 1, -(matrix.GetMatrixValue(0, 0) * matrix.GetMatrixValue(1, 2) -
            matrix.GetMatrixValue(0, 2) * matrix.GetMatrixValue(1, 0)));

        cofactorMatrix.SetMatrixValue(2, 2, matrix.GetMatrixValue(0, 0) * matrix.GetMatrixValue(1, 1) -
            matrix.GetMatrixValue(0, 1) * matrix.GetMatrixValue(1, 0));

        *determinant = (matrix.GetMatrixValue(0, 0) * matrix.GetMatrixValue(1, 1) * matrix.GetMatrixValue(2, 2) +
            matrix.GetMatrixValue(0, 1) * matrix.GetMatrixValue(1, 2) * matrix.GetMatrixValue(2, 0) +
            matrix.GetMatrixValue(1, 0) * matrix.GetMatrixValue(2, 1) * matrix.GetMatrixValue(0, 2)) -
            (matrix.GetMatrixValue(0, 2) * matrix.GetMatrixValue(1, 1) * matrix.GetMatrixValue(2, 0) +
                matrix.GetMatrixValue(0, 1) * matrix.GetMatrixValue(1, 0) * matrix.GetMatrixValue(2, 2) +
                matrix.GetMatrixValue(1, 2) * matrix.GetMatrixValue(2, 1) * matrix.GetMatrixValue(0, 0));

        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                result->SetMatrixValue(i, j, (1 / *determinant * cofactorMatrix.GetMatrixValue(j, i)));
            }
        }
    }
}