#pragma once
#include "LogicSystemECS.h"

class EffectPumpBehaviour : public BehaviourECS {
public:
	void update(Entity entity) override;

	EffectPumpBehaviour()
		: isPumpOn(true), hasForceAdded(false), timer(0.0f), offDuration(3.0f), onDuration(5.0f) {}

private:
	bool isPumpOn;
	bool hasForceAdded;
	float timer;
	float offDuration;
	float onDuration;
};