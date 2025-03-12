#pragma once
#include "LogicSystemECS.h"

class NavigationBehaviour : public BehaviourECS {
public:
    void update(Entity entity);
};