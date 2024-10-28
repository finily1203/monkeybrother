#include "filePath.h"


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
    // retrieves the executable path
    std::string execPath = GetExecutablePath();

    // retrieves the windowsConfig JSON file path
    std::string jsonPath = execPath.substr(0, execPath.find_last_of("\\/")) + "\\..\\..\\Sandbox\\Serialization\\windowConfig.json";

    return jsonPath;
}

// this function retrieves the entites JSON file
std::string FilePathManager::GetEntitiesJSONPath()
{
    // retrieves the executable path
    std::string execPath = GetExecutablePath();

    // retrieves the entities JSON file path
    std::string jsonPath = execPath.substr(0, execPath.find_last_of("\\/")) + "\\..\\..\\Sandbox\\Serialization\\entities.json";

    return jsonPath;
}

// this function retrieves the audio assets JSON file
std::string FilePathManager::GetAudioAssetsJSONPath()
{
    // retrieves the executable path
    std::string execPath = GetExecutablePath();

    // retrieves the audio assets JSON file path
    std::string jsonPath = execPath.substr(0, execPath.find_last_of("\\/")) + "\\..\\..\\Sandbox\\Serialization\\audioAssets.json";

    return jsonPath;
}
