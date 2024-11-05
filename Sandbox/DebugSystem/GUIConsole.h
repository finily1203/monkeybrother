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
			items.push_back(currentLog.str());
			currentLog.str("");
			currentLog.clear();

			// Check if we've exceeded the maximum number of logs
			if (items.size() > MAX_LOGS && autoDelete) {
				items.erase(items.begin(), items.begin() + (items.size() - MAX_LOGS));
			}
		}
		return *this;
	}

	void LoadConsoleConfigFromJSON(std::string const& filename);
	void SaveConsoleConfigToJSON(std::string const& filename);

private:
	//static const size_t MAX_LOGS = 1000;
	size_t MAX_LOGS;
	static Console* instance;
	static std::vector<std::string> items;
	static bool autoScroll;
	static bool autoDelete;
	static float lastScrollY;
	static std::ostringstream currentLog;

	Console() { LoadConsoleConfigFromJSON(FilePathManager::GetIMGUIConsoleJSONPath()); }
	void DrawImpl(const char* title); //ImGui console GUI format
};
