#include "../MathLibrary/vector2D.h"
#include "../MathLibrary/matrix3x3.h"

#ifndef MHMath_COLLISION_H_
#define MHMath_COLLISION_H_

namespace CollisionFramework {
	class Collision {
		struct AABB
		{

			myMath::Vector2D min;
			myMath::Vector2D max;
		};

		bool CollisionIntersection_RectRect(const AABB& aabb1,            //Input
			const myMath::Vector2D& vel1,           //Input 
			const AABB& aabb2,            //Input 
			const myMath::Vector2D& vel2,           //Input
			float& firstTimeOfCollision); //Output: the calculated value of tFirst, must be returned here

		struct LineSegment
		{
			myMath::Vector2D	m_pt0;						//End point P0
			myMath::Vector2D	m_pt1;						//End point P1
			myMath::Vector2D	m_normal;					//normalized outward normal
		};

		void BuildLineSegment(LineSegment& lineSegment,								//Line segment reference - input
			const myMath::Vector2D& p0,						//Point P0 - input
			const myMath::Vector2D& p1);						//Point P1 - input

		struct Circle
		{
			myMath::Vector2D  m_center;
			float	m_radius;
		};

		int CollisionIntersection_CircleLineSegment(const Circle& circle,			//Circle data - input
			const myMath::Vector2D& ptEnd,											//End circle position - input
			const LineSegment& lineSeg,												//Line segment - input
			myMath::Vector2D& interPt,												//Intersection point - output
			myMath::Vector2D& normalAtCollision,									//Normal vector at collision time - output
			float& interTime,														//Intersection time ti - output
			bool& checkLineEdges);													//The last parameter is for Extra Credits: when true => check collision with line segment edges

		int CheckMovingCircleToLineEdge(bool withinBothLines,						//Flag stating that the circle is starting from between 2 imaginary line segments distant +/- Radius respectively - input
			const Circle& circle,													//Circle data - input
			const myMath::Vector2D& ptEnd,											//End circle position - input
			const LineSegment& lineSeg,												//Line seCirclegment - input
			myMath::Vector2D& interPt,												//Intersection point - output
			myMath::Vector2D& normalAtCollision,									//Normal vector at collision time - output
			float& interTime);														//Intersection time ti - output

		void CollisionResponse_CircleLineSegment(const myMath::Vector2D& ptInter,				//Intersection position of the circle - input
			const myMath::Vector2D& normal,													//Normal vector of reflection on collision time - input
			myMath::Vector2D& ptEnd,															//Final position of the circle after reflection - output
			myMath::Vector2D& reflected);														//Normalized reflection vector direction - output

	};
}

#endif // MHMath_COLLISION_H_