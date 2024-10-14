#pragma once
#include "ECSCoordinator.h"
#include "GraphicsSystem.h"
#include "Debug.h"

enum SystemType { //For perfomance viewer
	AudioSystemType,
	WindowSystemType,
	DebugSystemType,
	GraphicsSystemType,
	ECSType
};

extern ECSCoordinator ecsCoordinator;
extern GraphicsSystem graphicsSystem;
extern DebugSystem debugSystem;