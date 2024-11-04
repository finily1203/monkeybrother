#pragma once
/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Ian Loi (ian.loi)
@team   :  MonkeHood
@course :  CSD2401
@file   :  vector3D.h
@brief  :  vector3D.h contains the class of vector3D and the declaration of all member and non-member functions
		   for doing the basic operations of what a 3D vector should do. The declared functions will do the
		   addition, subtraction between two 3D vectors, constructing a Vector3D class object, calculating
		   normalized vector, length of vector, distance between two vectors and more.

*Ian Loi (ian.loi) :
		- Declared functions of the vector3D class that will handle the different operations of a 3D
		  vector.

File Contributions: Ian Loi (100%)

*//*_________________________________________________________________________________________________-*/


namespace myMath
{
	class Vector3D
	{
	public:
		Vector3D();
		Vector3D(float, float, float);
		Vector3D(Vector3D const&);

		float GetX() const;
		float GetY() const;
		float GetZ() const;

		void SetX(float);
		void SetY(float);
		void SetZ(float);

		Vector3D& operator=(Vector3D const&);
		Vector3D& operator=(Vector3D&);

		Vector3D& operator+=(Vector3D const&);
		Vector3D& operator-=(Vector3D const&);
		Vector3D& operator*=(float);
		Vector3D& operator/=(float);

		Vector3D operator-() const;

	private:
		float x;
		float y;
		float z;
	};

	// non-member binary operation functions
	Vector3D operator+(Vector3D const&, Vector3D const&);
	Vector3D operator-(Vector3D const&, Vector3D const&);
	Vector3D operator*(Vector3D const&, float);
	Vector3D operator*(float, Vector3D const&);
	Vector3D operator/(Vector3D const&, float);

	// other non-member functions
	void NormalizeVector3D(Vector3D&, Vector3D const&);
	float LengthVector3D(Vector3D const&);
	float SquareLengthVector3D(Vector3D const&);
	float DistanceVector3D(Vector3D const&, Vector3D const&);
	float SquareDistanceVector3D(Vector3D const&, Vector3D const&);
	float DotProductVector3D(Vector3D const&, Vector3D const&);
	float CrossProductMagnitudeVector3D(Vector3D const&, Vector3D const&);
}
