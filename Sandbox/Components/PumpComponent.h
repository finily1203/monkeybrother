#pragma once
struct PumpComponent
{
	bool isPump;
	bool isAnimate;
	float pumpForce;
	PumpComponent() : isPump(false), isAnimate(false), pumpForce(0.f) {}
};