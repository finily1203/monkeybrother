#pragma once
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "../Serialization/jsonSerialization.h"
#include "../FilePaths/filePath.h"
#include <filesystem>



class AssetBrowser {
public:
	
	static void Initialise();
	static void Update();
	static void Cleanup();

private:
	static std::vector<std::string> assetNames;
	static bool showTextures;
	static bool showShaders;
	static bool showAudio;
	static bool showFonts;
};