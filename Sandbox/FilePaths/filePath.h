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
	static std::string GetAssetsJSONPath();
	static std::string GetIMGUIFontPath();
};
