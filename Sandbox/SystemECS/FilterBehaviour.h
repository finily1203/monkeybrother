#pragma once
#include "LogicSystemECS.h"

class FilterBehaviour : public BehaviourECS {
public:
	void update(Entity entity) override;

private:
	bool isClogged = false;
	std::chrono::time_point<std::chrono::steady_clock> collisionTime;
};