/*
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Lee Jing Wen (jingwen.lee)
@team   :  MonkeHood
@course :  CSD2401
@file   :  GUIAssetBrowser.h
@brief  :  This file contains the function declaration of ImGui asset browser

*Lee Jing Wen (jingwen.lee):
	- Integrated ImGui asset browser to allow users to view and interact with assets
	- Integrated to allow drag and drop, delete and update of assets

File Contributions: Lee Jing Wen (100%)

/*_______________________________________________________________________________________________________________*/

#pragma once
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "../Serialization/jsonSerialization.h"
#include "../FilePaths/filePath.h"
#include <filesystem>


//Class for ImGui asset browser in debugging mode
class AssetBrowser {
public:
	
	static void Initialise();
	static void Update();
	static void Cleanup();

	static std::string cutString(const std::string& str, float maxWidth);

private:
	static std::vector<std::string>* assetNames;
	static bool showTextures;
	static bool showShaders;
	static bool showAudio;
	static bool showFonts;
	static bool showPrefab;
};