#pragma once
#include "LogicSystemECS.h"

class ExitBehaviour : public BehaviourECS {
public:
	void update(Entity entity) override;
};
