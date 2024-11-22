#pragma once
#include "LogicSystemECS.h"

class PlayerBehaviour : public BehaviourECS {
public:
	void update(Entity entity, float dt) override;
};