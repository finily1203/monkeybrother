#pragma once
#include "ECSCoordinator.h"

enum CollisionSide {
	NONE,
	TOP,
	BOTTOM,
	LEFT,
	RIGHT,
	LEFTEDGE,
	RIGHTEDGE
};

class CollisionSystemECS
{
public:
	CollisionSystemECS() = default;

	struct AABB
	{
		glm::vec2 min;
		glm::vec2 max;
	};

	bool CollisionIntersection_RectRect(const AABB& platform,
		const glm::vec2& platformVel,
		const AABB& player,
		const glm::vec2& playerVel,
		float& firstTimeOfCollision);
	bool AABBSlopeCollision(Entity platform, Entity player, glm::vec2 velocity);

	CollisionSide circleRectCollision(float circleX, float circleY, float circleRadius, Entity platform);
};

class PhysicsSystemECS : public System
{
public:
	PhysicsSystemECS();

	void initialise() override;
	void update(float dt) override;
	void cleanup() override;

	glm::vec2 GetVelocity() const { return velocity; }
	bool GetAlrJumped() const { return alrJumped; }
	glm::vec2 GetCollisionPoint() const { return collisionPoint; }

	void SetVelocity(glm::vec2 newVelocity) { velocity = newVelocity; }
	void SetAlrJumped(bool newAlrJumped) { alrJumped = newAlrJumped; }
	void SetCollisionPoint(glm::vec2 newCollisionPoint) { collisionPoint = newCollisionPoint; }

	void ApplyGravity(Entity entity, float dt);
	Entity FindClosestPlatform(Entity player);
	//void Update(float friction);          // Updates physics per frame
	void HandleCollision(Entity closestPlatform, Entity player); // Handle collision logic
	void HandleSideCollision(int collisionSide, float circleRadius, Entity player, Entity closestPlatform);
	void HandleSlopeCollision(Entity closestPlatform, Entity player);

	void ApplyGravityAABB();
	void HandleAABBCollision(Entity player, Entity closestPlatform);
	void HandlePlayerInput(Entity player);

private:
	glm::vec2 velocity;
	glm::vec2 collisionPoint;
	float gravity;
	float jumpForce;
	float friction;
	bool alrJumped;

	CollisionSystemECS collisionSystem;
};

