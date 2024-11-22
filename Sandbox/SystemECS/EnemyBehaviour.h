#pragma once
#include "LogicSystemECS.h"

class EnemyBehaviour : public BehaviourECS {
public:
	void update(Entity entity, float dt) override;
};