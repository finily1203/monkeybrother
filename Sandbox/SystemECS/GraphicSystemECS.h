#pragma once
#include "ECSCoordinator.h"
#include "GraphicsSystem.h"

class GraphicSystemECS : public System
{
public:
	GraphicSystemECS() = default;

	void initialise() override;
	void update(float dt) override;
	void cleanup() override;

private:
};