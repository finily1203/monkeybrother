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
	static std::string GetExecutablePath();
	static std::string GetWindowConfigJSONPath();
	static std::string GetEntitiesJSONPath();
	static std::string GetIMGUIConsoleJSONPath();
	static std::string GetIMGUIViewportJSONPath();
	static std::string GetIMGUIDebugJSONPath();
	static std::string GetIMGUILayoutPath();
	static std::string GetIMGUIInspectorJSONPath();
	static std::string GetIMGUICreationJSONPath();
	static std::string GetIMGUIHierarchyJSONPath();
	static std::string GetAssetsJSONPath();
	static std::string GetIMGUIFontPath();
	static std::string GetPhysicsPath();
	static std::string GetSaveJSONPath(int& saveCount);
	static std::string GetSceneJSONPath();
	static std::string GetMainMenuJSONPath();
	static std::string GetPauseMenuJSONPath();
	static std::string GetOptionsMenuJSONPath();
	static std::string GetIntroCutsceneJSONPath();	
};