/*
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Lew Zong Han Owen (z.lew)
@team   :  MonkeHood
@course :  CSD2401
@file   :  GUIHierarchyList.h
@brief  :  This file contains the function declaration of ImGui hierarchy list system

*Lew Zong Han Owen (z.lew) :
		- Integrated ImGui hierarchy list to display all existing entities. Allow individual entity editing

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

#define MAXTEXTSIZE 1000

class HierarchyList {
public:

	static void Initialise();
	static void Update();
	static void Cleanup();

	static void LoadHierarchyListFromJSON(std::string const& filename);

private:
	static float objAttributeSliderMaxLength;
	static char textBuffer[MAXTEXTSIZE];
};