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
#include "EngineDefinitions.h"
#include "ECSCoordinator.h"
#include "GraphicsSystem.h"
#include "Debug.h"
#include "AssetsManager.h"
#include "AudioSystem.h"
#include "CameraSystem2D.h"
#include "FontSystem.h"
#include "LayerManager.h"
#include "CutsceneSystem.h"
#include "GUIGridSystem.h"


enum SystemType { //For perfomance viewer
	AudioSystemType,
	WindowSystemType,
	DebugSystemType,
	GraphicsSystemType,
	CameraType,
	ECSType,
	AssetsManagerType,
	FontType,
	LayerManagerType,
	CutsceneSystemType
};

extern AudioSystem audioSystem;
extern AssetsManager assetsManager;
extern ECSCoordinator ecsCoordinator;
extern GraphicsSystem graphicsSystem;
extern CameraSystem2D cameraSystem;
extern DebugSystem debugSystem;
extern FontSystem fontSystem;
extern LayerManager layerManager;
extern CutsceneSystem cutsceneSystem;
extern GridSystem gridSystem;