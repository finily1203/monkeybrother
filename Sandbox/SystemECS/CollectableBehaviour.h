#pragma once
#include "LogicSystemECS.h"

class CollectableBehaviour : public BehaviourECS {
public:
	void update(Entity entity) override;
};