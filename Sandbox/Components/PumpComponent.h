#pragma once
struct PumpComponent
{
	bool isPump;

	float pumpForce;
	PumpComponent() : isPump(false), pumpForce(0.f) {}
};