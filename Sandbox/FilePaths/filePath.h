/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Ian Loi (ian.loi)
@team   :  MonkeHood
@course :  CSD2401
@file   :  filePath.h
@brief  :  filePath.h contains the FilePathManager class, this class handles all the different
		   files that we need to load dynamically. The files include from the executable path and
		   all the JSON files.

*Ian Loi (ian.loi) :
		- Created the FilePathManager class with its member functions to load all the file paths
		  dynamically.

File Contributions: Ian Loi (100%)

*//*__________________________________________________________________________________________________*/
#pragma once

#define NOMINMAX
#include <string>
#include <Windows.h>


class FilePathManager
{
public:
	// retrieve the executable file path
	static std::string GetExecutablePath();
	// retrieve the window config JSON file path
	static std::string GetWindowConfigJSONPath();
	// retrieve the entities JSON file path
	static std::string GetEntitiesJSONPath();
	// retrieve the IMGUI console JSON file path
	static std::string GetIMGUIConsoleJSONPath();
	// retrieve the IMGUI viewport JSON file path
	static std::string GetIMGUIViewportJSONPath();
	// retrieve the IMGUI debug JSON file path
	static std::string GetIMGUIDebugJSONPath();
	// retrieve the IMGUI layout JSON path
	static std::string GetIMGUILayoutPath();
	// retrieve the IMGUI inspector JSON path
	static std::string GetIMGUIInspectorJSONPath();
	// retrieve the IMGUI creation JSON path
	static std::string GetIMGUICreationJSONPath();
	// retrieve the IMGUI hierarchy JSON path
	static std::string GetIMGUIHierarchyJSONPath();
	// retrieve the assets JSON path
	static std::string GetAssetsJSONPath();
	// retrieve the IMGUI font path
	static std::string GetIMGUIFontPath();
	// retrieve the physics JSON path
	static std::string GetPhysicsPath();
	// retrieve the save JSON path
	static std::string GetSaveJSONPath(int& saveCount);
	// retrieve the scene JSON path
	static std::string GetSceneJSONPath();
	// retrieve the main menu JSON path
	static std::string GetMainMenuJSONPath();
	// retrieve the pause menu JSON path
	static std::string GetPauseMenuJSONPath();
	// retrieve the options menu JSON path
	static std::string GetOptionsMenuJSONPath();
	// retrieve the layer JSON path
	static std::string GetLayerJSONPath();
	// retrieve the audio settings JSON path
	static std::string GetAudioSettingsJSONPath();
};