/*
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Lew Zong Han Owen (z.lew)
@team   :  MonkeHood
@course :  CSD2401
@file   :  GUIInspector.h
@brief  :  This file contains the function declaration of ImGui Inspector system

*Lew Zong Han Owen (z.lew) :
		- Integrated ImGui inspector window to capture selected entity's transformation data
		- Integrated entity to mouse interaction to drag, scale, rotate, and delete
		- Integrated the feature to allow re-assigning of pre-existing entity behavior

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

#define constextptr MAXTEXTSIZE 1000

class Inspector {
public:

	static void Initialise();

	static void Update();

	static void Cleanup();

	static ImVec2 getMouseWorldPos() { return mouseWorldPos; }

	static void RenderInspectorWindow(ECSCoordinator& ecsCoordinator, int selectedEntityID);

	static void LoadInspectorFromJSON(std::string const& filename);

	static int selectedEntityID;
	static int draggedEntityID;
	static bool isSelectingEntity;

	static std::vector<std::string>* assetNames;
	static float objAttributeSliderMaxLength;
	static char textBuffer[MAXTEXTSIZE];
	static ImVec2 mouseWorldPos;
	static std::vector<std::pair<int, std::string>>* overlappingEntities;
};

