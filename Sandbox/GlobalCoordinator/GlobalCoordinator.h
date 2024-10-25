/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   GlobalCoordinator.h
@brief:  This header file is declares the external variables used mainly for
		 interactions between ECS and Graphics System.
		 Joel Chu (c.weiyuan): Defined the two global variables to be used outside
							   of the ECS and Graphics System.
							   100%
*//*___________________________________________________________________________-*/
#pragma once
#include "ECSCoordinator.h"
#include "GraphicsSystem.h"
#include "Debug.h"
#include "AssetsManager.h"

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
extern AssetsManager assetsManager;