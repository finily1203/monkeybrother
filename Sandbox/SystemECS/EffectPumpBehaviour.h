#pragma once
#include "LogicSystemECS.h"

class EffectPumpBehaviour : public BehaviourECS {
public:
	void update(Entity entity) override;

	EffectPumpBehaviour()
		: hasForceAdded(false), timer(0.0f), offDuration(3.0f), onDuration(5.0f) {}

	void managePumpSound(bool isPumpOn);

private:
	bool hasForceAdded;
	float timer;
	float offDuration;
	float onDuration;
};