/*
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Lew Zong Han Owen (z.lew)
@team   :  MonkeHood
@course :  CSD2401
@file   :  GUIConsole.h
@brief  :  This file contains the function declaration of ImGui console window

*Lew Zong Han Owen (z.lew) :
		- Integrated ImGui console to allow an alternative channel to output debug information

*Ian Loi (ian.loi) :
		- Integrated serialization & deserialization functions to initialize variables and save from and to
		  json file

File Contributions: Lew Zong Han Owen (90%)
					Ian Loi           (10%)

/*_______________________________________________________________________________________________________________*/
#pragma once
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "../Serialization/jsonSerialization.h"
#include "../FilePaths/filePath.h"

#include <iostream>
#include <vector>
#include <sstream>

//Class for ImGui console window in debugging mode
class Console {
public:

	static void Update(const char* title);

	static void Cleanup();

	~Console();

	static Console& GetLog(); //Called for logging

	Console(Console const&) = delete; //Prevents copying
	void operator=(Console const&) = delete; //Prevent reassigning

	template<typename T>
	Console& operator<<(const T& value) { //Handle general output
		currentLog << value;
		return *this;
	}

	Console& operator<<(std::ostream& (*manip)(std::ostream&)) { //Handle std::endl
		if (manip == static_cast<std::ostream & (*)(std::ostream&)>(std::endl)) {
			items->push_back(currentLog.str());
			currentLog.str("");
			currentLog.clear();

			// Check if we've exceeded the maximum number of logs
			if (items->size() > MAX_LOGS && autoDelete) {
				items->erase(items->begin(), items->begin() + (items->size() - MAX_LOGS));
			}
		}
		return *this;
	}

	void LoadConsoleConfigFromJSON(std::string const& filename); //Load console configuration from JSON file
	void SaveConsoleConfigToJSON(std::string const& filename);  //Saves console configuration to a JSON file

	void DebugPrintState() {
		std::cout << "\n=== Console Current State ===" << std::endl;
		std::cout << "Instance exists: " << (instance != nullptr ? "Yes" : "No") << std::endl;
		std::cout << "Items count: " << items->size() << std::endl;
		std::cout << "Auto-scroll enabled: " << (autoScroll ? "Yes" : "No") << std::endl;
		std::cout << "Auto-delete enabled: " << (autoDelete ? "Yes" : "No") << std::endl;
		std::cout << "Last scroll position: " << lastScrollY << std::endl;
		std::cout << "Current log buffer: '" << currentLog.str() << "'" << std::endl;
		std::cout << "===========================" << std::endl;
	}

private:
	static size_t MAX_LOGS;
	static Console* instance;
	static std::vector<std::string>* items;
	static bool autoScroll;
	static bool autoDelete;
	static float lastScrollY;
	static std::ostringstream currentLog;

	Console() { LoadConsoleConfigFromJSON(FilePathManager::GetIMGUIConsoleJSONPath()); if (!items) { items = new std::vector<std::string>(); } }
	void DrawImpl(const char* title); //ImGui console GUI format
};