#pragma once
#include "LogicSystemECS.h"

class FilterBehaviour : public BehaviourECS {
public:
	void update(Entity entity) override;

private:
	bool isClogged = false;
};