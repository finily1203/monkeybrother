#include "Collision.h"
#include "../GLFWFunctions/GlfwFunctions.h"

namespace CollisionFramework {

	bool Collision::CollisionIntersection_RectRect(const AABB& aabb1,          //Input
		const myMath::Vector2D& vel1,         //Input 
		const AABB& aabb2,          //Input 
		const myMath::Vector2D& vel2,         //Input
		float& firstTimeOfCollision) //Output: the calculated value of tFirst, below, must be returned here
	{

		if (aabb1.max.GetX() < aabb2.min.GetX() || aabb1.max.GetY() < aabb2.min.GetY() || aabb1.min.GetX() > aabb2.max.GetX() || aabb1.min.GetY() > aabb2.max.GetY()) { // if no overlap
			//Step 2: Initialize and calculate new velocity of Vb
			float tFirst = 0;
			float tLast = GLFWFunctions::fps;

			myMath::Vector2D Vb;
			myMath::Vector2D(vel1.GetX() - vel2.GetX(), vel1.GetY() - vel2.GetY());

			if (Vb.GetX() < 0) {
				//case 1
				if (aabb1.min.GetX() > aabb2.max.GetX()) {
					return 0;
				}
				//case 4
				if (aabb1.max.GetX() < aabb2.min.GetX()) {
					tFirst = fmax((aabb1.max.GetX() - aabb2.min.GetX()) / Vb.GetX(), tFirst);
				}
				if (aabb1.min.GetX() < aabb2.max.GetX()) {
					tLast = fmin((aabb1.min.GetX() - aabb2.max.GetX()) / Vb.GetX(), tLast);
				}
			}
			else if (Vb.GetX() > 0) {
				// case 2
				if (aabb1.min.GetX() > aabb2.max.GetX()) {
					tFirst = fmax((aabb1.min.GetX() - aabb2.max.GetX()) / Vb.GetX(), tFirst);
				}
				if (aabb1.max.GetX() > aabb2.min.GetX()) {
					tLast = fmin((aabb1.max.GetX() - aabb2.min.GetX()) / Vb.GetX(), tLast);
				}
				//case 3
				if (aabb1.max.GetX() < aabb2.min.GetX()) {
					return 0;
				}
			}
			else {
				// case 5
				if (aabb1.max.GetX() < aabb2.min.GetX()) {
					return 0;
				}
				else if (aabb1.min.GetX() > aabb2.max.GetX()) {
					return 0;
				}
			}

			// case 6
			if (tFirst > tLast) {
				return 0;
			}

			if (Vb.GetY() < 0) {
				//case 1
				if (aabb1.min.GetY() > aabb2.max.GetY()) {
					return 0;
				}
				//case 4
				if (aabb1.max.GetY() < aabb2.min.GetY()) {
					tFirst = fmax((aabb1.max.GetY() - aabb2.min.GetY()) / Vb.GetY(), tFirst);
				}
				if (aabb1.min.GetY() < aabb2.max.GetY()) {
					tLast = fmin((aabb1.min.GetY() - aabb2.max.GetY()) / Vb.GetY(), tLast);
				}
			}
			else if (Vb.GetY() > 0) {
				// case 2
				if (aabb1.min.GetY() > aabb2.max.GetY()) {
					tFirst = fmax((aabb1.min.GetY() - aabb2.max.GetY()) / Vb.GetY(), tFirst);
				}
				if (aabb1.max.GetY() > aabb2.min.GetY()) {
					tLast = fmin((aabb1.max.GetY() - aabb2.min.GetY()) / Vb.GetY(), tLast);
				}
				//case 3
				if (aabb1.max.GetY() < aabb2.min.GetY()) {
					return 0;
				}
			}
			else {
				// case 5
				if (aabb1.max.GetY() < aabb2.min.GetY()) {
					return 0;
				}
				else if (aabb1.min.GetY() > aabb2.max.GetY()) {
					return 0;
				}
			}
			// case 6
			if (tFirst > tLast) {
				return 0;
			}
			firstTimeOfCollision = tFirst;
			return 1;
		}
		else {
			return 1;
		}
	}

	void Collision::BuildLineSegment(LineSegment& lineSegment,
		const myMath::Vector2D& p0,
		const myMath::Vector2D& p1)
	{
		// Set the line segment's points
		lineSegment.m_pt0 = p0;
		lineSegment.m_pt1 = p1;

		// Calculate the direction vector from p0 to p1
		myMath::Vector2D direction = { p1.GetX() - p0.GetX(), p1.GetY() - p0.GetY() };
		lineSegment.m_normal = { direction.GetY(), -direction.GetX() };

		// Normalize the normal vector
		myMath::NormalizeVector2D(lineSegment.m_normal, lineSegment.m_normal);
	}

	int Collision::CollisionIntersection_CircleLineSegment(const Circle& circle,
		const myMath::Vector2D& ptEnd,
		const LineSegment& lineSeg,
		myMath::Vector2D& interPt,
		myMath::Vector2D& normalAtCollision,
		float& interTime,
		bool& checkLineEdges)
	{

		myMath::Vector2D N = lineSeg.m_normal;
		myMath::Vector2D Bs = circle.m_center;
		float R = circle.m_radius;

		float NBs = myMath::DotProductVector2D(Bs, N);
		float NP0 = myMath::DotProductVector2D(lineSeg.m_pt0, N);

		if (NBs - NP0 <= -R) {
			myMath::Vector2D V{ ptEnd - Bs };
			myMath::Vector2D M{ V.GetY(), -V.GetX() }; // M is the outward normal to Velocity V

			myMath::Vector2D P0_P{ lineSeg.m_pt0 - R * N };
			myMath::Vector2D P1_P{ lineSeg.m_pt1 - R * N };

			myMath::Vector2D BsP0{ P0_P - Bs };
			myMath::Vector2D BsP1{ P1_P - Bs };

			if (myMath::DotProductVector2D(M, BsP0) * myMath::DotProductVector2D(M, BsP1) < 0) {

				interTime = (myMath::DotProductVector2D(N, lineSeg.m_pt0) - myMath::DotProductVector2D(N, Bs) - R) / myMath::DotProductVector2D(N, V);
				if (0 <= interTime && interTime <= 1) {
					interPt = Bs + V * interTime;
					normalAtCollision = -N;
					return 1;
				}
			}
			else {
				checkLineEdges = false;
				return CheckMovingCircleToLineEdge(checkLineEdges, circle, ptEnd, lineSeg, interPt, normalAtCollision, interTime); // Check for collision with line segment edges
			}


		}
		else if (myMath::DotProductVector2D(N, Bs) - myMath::DotProductVector2D(N, lineSeg.m_pt0) >= R) {
			myMath::Vector2D V{ ptEnd - Bs };
			myMath::Vector2D M{ V.GetY(), -V.GetX() };

			myMath::Vector2D P0_P{ lineSeg.m_pt0 + R * N };
			myMath::Vector2D P1_P{ lineSeg.m_pt1 + R * N };

			myMath::Vector2D BsP0{ P0_P - Bs };
			myMath::Vector2D BsP1{ P1_P - Bs };

			if (myMath::DotProductVector2D(M, BsP0) * myMath::DotProductVector2D(M, BsP1) < 0) {

				interTime = (myMath::DotProductVector2D(N, lineSeg.m_pt0) - myMath::DotProductVector2D(N, Bs) + R) / myMath::DotProductVector2D(N, V);
				if (0 <= interTime && interTime <= 1) {
					interPt = Bs + V * interTime;
					normalAtCollision = N;

					return 1;
				}
			}
			else {
				checkLineEdges = false;
				return CheckMovingCircleToLineEdge(checkLineEdges, circle, ptEnd, lineSeg, interPt, normalAtCollision, interTime); // Check for collision with line segment edges
			}


		}
		else {
			checkLineEdges = true;
			return CheckMovingCircleToLineEdge(checkLineEdges, circle, ptEnd, lineSeg, interPt, normalAtCollision, interTime); // Check for collision with line segment edges
		}

		return 0;
	}

	int Collision::CheckMovingCircleToLineEdge(bool withinBothLines,
		const Circle& circle,
		const myMath::Vector2D& ptEnd,
		const LineSegment& lineSeg,
		myMath::Vector2D& interPt,
		myMath::Vector2D& normalAtCollision,
		float& interTime)
	{
		// Setting up the variables for easier calculation
		myMath::Vector2D N = lineSeg.m_normal;
		myMath::Vector2D Bs = circle.m_center;
		float R = circle.m_radius;
		myMath::Vector2D V{ ptEnd - Bs };
		myMath::Vector2D M{ V.GetY(), -V.GetX() };

		// Normalizing the vectors V and M
		myMath::Vector2D NV, NM;
		myMath::NormalizeVector2D(NV, V);
		myMath::NormalizeVector2D(NM, M);

		myMath::Vector2D BsP0{ lineSeg.m_pt0 - Bs };
		myMath::Vector2D BsP1{ lineSeg.m_pt1 - Bs };
		myMath::Vector2D P0P1{ lineSeg.m_pt1 - lineSeg.m_pt0 };

		if (withinBothLines) {
			if (myMath::DotProductVector2D(BsP0, P0P1) > 0) {

				float m = myMath::DotProductVector2D(BsP0, NV);

				if (m > 0) {
					float dist0 = myMath::DotProductVector2D(BsP0, NM);

					if (fabs(dist0) > R) {
						return 0;
					}

					float s = std::sqrt(R * R - dist0 * dist0);

					interTime = (m - s) / myMath::LengthVector2D(V);

					if (interTime <= 1.f) {
						interPt = Bs + V * interTime;
						myMath::Vector2D P0Bi{ interPt - lineSeg.m_pt0 };
						myMath::NormalizeVector2D(P0Bi, P0Bi);
						normalAtCollision = P0Bi;
						return 1;
					}
				}
				else return 0;
			}
			else {
				float m = myMath::DotProductVector2D(BsP1, NV);
				if (m > 0) {
					float dist1 = myMath::DotProductVector2D(BsP1, NM);

					if (fabs(dist1) > R) {
						return 0;
					}

					float s = std::sqrt(R * R - dist1 * dist1);

					interTime = (m - s) / myMath::LengthVector2D(V);

					if (interTime <= 1.f) {
						interPt = Bs + V * interTime;
						myMath::Vector2D P1Bi{ interPt - lineSeg.m_pt1 };
						myMath::NormalizeVector2D(P1Bi, P1Bi);
						normalAtCollision = P1Bi;
						return 1;
					}
				}
				else return 0;
			}

		}
		else {
			bool P0Side = false;

			float dist0 = myMath::DotProductVector2D(BsP0, NM);
			float dist1 = myMath::DotProductVector2D(BsP1, NM);

			float dist0_absoluteValue = fabs(dist0);
			float dist1_absoluteValue = fabs(dist1);

			if (dist0_absoluteValue > R && dist1_absoluteValue > R) {
				return 0;
			}
			else if (dist0_absoluteValue <= R && dist1_absoluteValue <= R) {
				float m0 = myMath::DotProductVector2D(BsP0, NV);
				float m1 = myMath::DotProductVector2D(BsP1, NV);
				float m0_absoluteValue = fabs(m0);
				float m1_absoluteValue = fabs(m1);
				if (m0_absoluteValue < m1_absoluteValue) {
					P0Side = true;
				}
				else {
					P0Side = false;
				}
			}
			else if (dist0_absoluteValue <= R) {
				P0Side = true;
			}
			else {
				P0Side = false;
			}

			if (P0Side) {
				float m = myMath::DotProductVector2D(BsP0, NV);
				if (m < 0) {
					return 0;
				}
				else {
					float s = std::sqrt(R * R - dist0 * dist0);

					interTime = (m - s) / myMath::LengthVector2D(V);

					if (interTime <= 1.f) {
						interPt = Bs + V * interTime;
						myMath::Vector2D P0Bi{ interPt - lineSeg.m_pt0 };
						myMath::NormalizeVector2D(P0Bi, P0Bi);

						normalAtCollision = P0Bi;

						return 1;
					}
				}
			}
			else {
				float m = myMath::DotProductVector2D(BsP1, NV);
				if (m < 0) {
					return 0;
				}
				else {
					float s = std::sqrt(R * R - dist1 * dist1);

					interTime = (m - s) / myMath::LengthVector2D(V);

					if (interTime <= 1.f) {
						interPt = Bs + V * interTime;
						myMath::Vector2D P1Bi{ interPt - lineSeg.m_pt1 };
						myMath::NormalizeVector2D(P1Bi, P1Bi);

						normalAtCollision = P1Bi;
						return 1;
					}
				}
			}
		}

		return 0; // no collision
	}

	void Collision::CollisionResponse_CircleLineSegment(const myMath::Vector2D& ptInter,
		const myMath::Vector2D& normal,
		myMath::Vector2D& ptEnd,
		myMath::Vector2D& reflected)
	{
		reflected = (ptEnd - ptInter) - 2 * myMath::DotProductVector2D(ptEnd - ptInter, normal) * normal;

		// Normalize the reflected vector
		myMath::NormalizeVector2D(reflected, reflected);
		ptEnd = ptInter + reflected;
	}
} // namespace CollisionFramework