#pragma once
#include "LogicSystemECS.h"

class MovPlatformBehaviour : public BehaviourECS {
public:
	void update(Entity entity) override;
};