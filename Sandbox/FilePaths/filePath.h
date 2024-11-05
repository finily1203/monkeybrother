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
	static std::string GetAssetsJSONPath();
	static std::string GetIMGUIFontPath();
	static std::string GetPhysicsPath();
	static std::string GetSaveJSONPath(int& saveCount);
};
