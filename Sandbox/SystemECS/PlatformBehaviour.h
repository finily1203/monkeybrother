#pragma once
#include "LogicSystemECS.h"

class PlatformBehaviour : public BehaviourECS {
public:
	void update(Entity entity) override;
};