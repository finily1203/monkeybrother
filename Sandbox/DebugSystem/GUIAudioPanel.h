/*
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Owen Lew (z.lew)
@team   :  MonkeHood
@course :  CSD2401
@file   :  GUIAudioPanel.h
@brief  :  This file contains the function declaration of the audio panel in the debug system

*Owen Lew (z.lew):
		- creates the audio panel for the debug system
		- users able to see the number of channels and the current music playing in the channel

File Contributions: Owen Lew (100%)

/*_______________________________________________________________________________________________________________*/

#pragma once
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "../Serialization/jsonSerialization.h"
#include "../FilePaths/filePath.h"

class AudioPanel {
public:
	static void Initialise();
	static void Update();
	static void Cleanup();
};