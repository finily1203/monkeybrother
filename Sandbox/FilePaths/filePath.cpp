/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Ian Loi (ian.loi)
@team   :  MonkeHood
@course :  CSD2401
@file   :  filePath.cpp
@brief  :  filePath.cpp contains all the definitions of the member functions of FilePathManager. The
           functions here will retrieve the executable file path and all the different JSON file paths.

*Ian Loi (ian.loi) :
        - Created the definitions of FilePathManager class member functions that will retrieve the executable
          file path and JSON file paths.

File Contributions: Ian Loi (100%)

*//*__________________________________________________________________________________________________*/
#include "filePath.h"
#include <filesystem>


// this function retrieves the executable path based on your desktop
// dynamic path retrieval
std::string FilePathManager::GetExecutablePath()
{
    char buffer[MAX_PATH];

    // this is a windows API function that will retrieve the fully
    // qualified path of the executable file 
    GetModuleFileNameA(nullptr, buffer, MAX_PATH);

    // initialize the full executable path to fullPath
    std::string fullPath(buffer);

    // return the directory path by extracting a substring from the
    // fullPath
    return fullPath.substr(0, fullPath.find_last_of("\\/"));
}

// this function retrieves the windowsConfig JSON file
std::string FilePathManager::GetWindowConfigJSONPath()
{
    std::filesystem::path execPath = GetExecutablePath();
    std::filesystem::path jsonPath = execPath.parent_path() / "Sandbox" / "assets" / "json" / "windowConfig.json";

    std::string jsonPathString = jsonPath.string();

    return jsonPathString;
}

// this function retrieves the entites JSON file
std::string FilePathManager::GetEntitiesJSONPath()
{
    std::filesystem::path execPath = GetExecutablePath();
    std::filesystem::path jsonPath = execPath.parent_path() / "Sandbox" / "assets" / "json" / "entities.json";

    std::string jsonPathString = jsonPath.string();

    return jsonPathString;
}

// this function retrieves the IMGUI console JSON file
std::string FilePathManager::GetIMGUIConsoleJSONPath()
{
    std::filesystem::path execPath = GetExecutablePath();
    std::filesystem::path jsonPath = execPath.parent_path() / "Sandbox" / "assets" / "json" / "imguiConsole.json";

    std::string jsonPathString = jsonPath.string();

    return jsonPathString;
}

// this function retrieves the IMGUI viewport JSON file
std::string FilePathManager::GetIMGUIViewportJSONPath()
{
    std::filesystem::path execPath = GetExecutablePath();
    std::filesystem::path jsonPath = execPath.parent_path() / "Sandbox" / "assets" / "json" / "imguiViewport.json";

    std::string jsonPathString = jsonPath.string();

    return jsonPathString;
}

// this function retrieves the IMGUI Creation JSON file
std::string FilePathManager::GetIMGUICreationJSONPath()
{
    std::filesystem::path execPath = GetExecutablePath();
    std::filesystem::path jsonPath = execPath.parent_path() / "Sandbox" / "assets" / "json" / "imguiCreation.json";

    std::string jsonPathString = jsonPath.string();

    return jsonPathString;
}

// this function retrieves the IMGUI Layout JSON file
std::string FilePathManager::GetIMGUILayoutPath() {
    std::filesystem::path execPath = GetExecutablePath();
    std::filesystem::path jsonPath = execPath.parent_path() / "Sandbox" / "assets" / "imgui" / "imgui_layout.ini";

    std::string jsonPathString = jsonPath.string();

    return jsonPathString;
}

// this function retrieves the IMGUI debug JSON file
std::string FilePathManager::GetIMGUIDebugJSONPath()
{
    std::filesystem::path execPath = GetExecutablePath();
    std::filesystem::path jsonPath = execPath.parent_path() / "Sandbox" / "assets" / "json" / "imguiDebug.json";

    std::string jsonPathString = jsonPath.string();

    return jsonPathString;
}

// this function retrieves the IMGUI Hierarchy JSON file
std::string FilePathManager::GetIMGUIHierarchyJSONPath()
{
    std::filesystem::path execPath = GetExecutablePath();
    std::filesystem::path jsonPath = execPath.parent_path() / "Sandbox" / "assets" / "json" / "imguiHierarchy.json";

    std::string jsonPathString = jsonPath.string();

    return jsonPathString;
}

std::string FilePathManager::GetIMGUIInspectorJSONPath()
{
    std::filesystem::path execPath = GetExecutablePath();
    std::filesystem::path jsonPath = execPath.parent_path() / "Sandbox" / "assets" / "json" / "imguiInspector.json";

    std::string jsonPathString = jsonPath.string();

    return jsonPathString;
}

// this function retrieves the audio assets JSON file
std::string FilePathManager::GetAssetsJSONPath()
{
    std::filesystem::path execPath = GetExecutablePath();
    std::filesystem::path jsonPath = execPath.parent_path() / "Sandbox" / "assets" / "json" / "assets.json";

    std::string jsonPathString = jsonPath.string();

    return jsonPathString;
}

// this function retrieves the font file for IMGUI
std::string FilePathManager::GetIMGUIFontPath()
{
    std::filesystem::path execPath = GetExecutablePath();
    std::filesystem::path jsonPath = execPath.parent_path() / "Sandbox" / "assets" / "fonts" / "liberation-mono.ttf";

    std::string jsonPathString = jsonPath.string();

    return jsonPathString;
}

// this function retrieves the physics JSON file
std::string FilePathManager::GetPhysicsPath()
{
    std::filesystem::path execPath = GetExecutablePath();
    std::filesystem::path jsonPath = execPath.parent_path() / "Sandbox" / "assets" / "json" / "physicsConfig.json";

    std::string jsonPathString = jsonPath.string();

    return jsonPathString;
}

// this function retrieves the save JSON file
std::string FilePathManager::GetSaveJSONPath(int& saveCount)
{
    std::filesystem::path execPath = GetExecutablePath();
    std::string saveFile = "save" + std::to_string(saveCount) + ".json";
    std::filesystem::path jsonPath = execPath.parent_path() / "Sandbox" / "assets" / "json" / saveFile;

    std::string jsonPathString = jsonPath.string();

    return jsonPathString;
}

// this function retrieves the scene JSON file
std::string FilePathManager::GetSceneJSONPath()
{
    std::filesystem::path execPath = GetExecutablePath();
    std::filesystem::path jsonPath = execPath.parent_path() / "Sandbox" / "assets" / "json" / "scene.json";

    std::string jsonPathString = jsonPath.string();

    return jsonPathString;
}

// this function retrieves the main menu JSON file
std::string FilePathManager::GetMainMenuJSONPath()
{
    std::filesystem::path execPath = GetExecutablePath();
    std::filesystem::path jsonPath = execPath.parent_path() / "Sandbox" / "assets" / "json" / "mainMenu.json";

    std::string jsonPathString = jsonPath.string();

    return jsonPathString;
}

// this function retrieves the pause menu JSON file
std::string FilePathManager::GetPauseMenuJSONPath()
{
    std::filesystem::path execPath = GetExecutablePath();
    std::filesystem::path jsonPath = execPath.parent_path() / "Sandbox" / "assets" / "json" / "pauseMenu.json";

    std::string jsonPathString = jsonPath.string();

    return jsonPathString;
}

// this function retrieves the optionsmenu JSON file
std::string FilePathManager::GetOptionsMenuJSONPath()
{
    std::filesystem::path execPath = GetExecutablePath();
    std::filesystem::path jsonPath = execPath.parent_path() / "Sandbox" / "assets" / "json" / "optionsMenu.json";

    std::string jsonPathString = jsonPath.string();

    return jsonPathString;
}

// this function retrieves the layer JSON file
std::string FilePathManager::GetLayerJSONPath()
{
    std::filesystem::path execPath = GetExecutablePath();
    std::filesystem::path jsonPath = execPath.parent_path() / "Sandbox" / "assets" / "json" / "layer.json";

    std::string jsonPathString = jsonPath.string();

    return jsonPathString;
}

// this function retrieves the audio settings JSON file
std::string FilePathManager::GetAudioSettingsJSONPath()
{
    std::filesystem::path execPath = GetExecutablePath();
    std::filesystem::path jsonPath = execPath.parent_path() / "Sandbox" / "assets" / "json" / "audioSettings.json";

    std::string jsonPathString = jsonPath.string();

    return jsonPathString;
}