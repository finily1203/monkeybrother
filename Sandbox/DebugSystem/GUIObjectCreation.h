/*
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Lew Zong Han Owen (z.lew)
@team   :  MonkeHood
@course :  CSD2401
@file   :  GUIObjectCreation.cpp
@brief  :  This file contains the function definition of ImGui object creation system

*Lew Zong Han Owen (z.lew) :
		- Integrated ImGui Object Creation system to allow custom game objects to be created by inputing object-specific
		  properties' data

File Contributions: Lew Zong Han Owen (100%)

/*_______________________________________________________________________________________________________________*/
#pragma once
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "../Serialization/jsonSerialization.h"
#include "../FilePaths/filePath.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h> 
#include "GlobalCoordinator.h"

#define MAXNAMELENGTH 15
#define MAXBUFFERSIZE 8
#define MAXTEXTSIZE 1000
#define MAXOBJTYPE 4

class ObjectCreation {
public:
	static void Initialise();

	static void Update();

	static void Cleanup();

	static void ObjectCreationCondition(const char* items[], int itemIndex, Entity entityObj, std::string entityId);

	static void LoadObjectCreationFromJSON(std::string const& filename);
private:

	static int objCount;
	static float objAttributeSliderMaxLength;
	static float objAttributeSliderMidLength;
	static float objAttributeSliderMinLength;
	static int currentItem;
	static char sigBuffer[MAXNAMELENGTH];
	static char textBuffer[MAXTEXTSIZE];
	static char textScaleBuffer[MAXBUFFERSIZE];
	static char xCoordinatesBuffer[MAXBUFFERSIZE];
	static char yCoordinatesBuffer[MAXBUFFERSIZE];
	static char xOrientationBuffer[MAXBUFFERSIZE];
	static char yOrientationBuffer[MAXBUFFERSIZE];
	static float xCoordinates;
	static float yCoordinates;
	static float xOrientation;
	static float yOrientation;
	static float textScale;
	static float textBoxWidth;
	static float textScaleMinLimit;
	static float coordinateMaxLimitsX;
	static float coordinateMaxLimitsY;
	static float coordinateMinLimitsX;
	static float coordinateMinLimitsY;
	static float orientationMaxLimit;
	static float orientationMinLimit;
	static int numEntitiesToCreate;
	static int minEntitesToCreate;
	static char numBuffer[MAXBUFFERSIZE];
	static float defaultObjScaleX;
	static float defaultObjScaleY;
	static float textBoxMaxLimit;
	static float textBoxMinLimit;

};