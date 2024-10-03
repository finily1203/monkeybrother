#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h> 
#include "GlfwFunctions.h"

#include "Systems.h"
#include "matrix3x3.h"
#include "GraphicsSystem.h"
#include "WindowSystem.h"
#include "SystemManager.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>
#include <unordered_map>
#include <csignal>
#include <cstdlib>

class DebugSystem {
public:
	ImVec4 clear_color = ImVec4(.45f, .45f, .45f, 1.00f);
	DebugSystem();
	~DebugSystem();

	void Initialise();
	void Update();
	void Cleanup();

	static void StartLoop(); //Start record game loop time

	static void StartSystemTiming(const char* systemName); //Start record system loop time

	static void EndSystemTiming(const char* systemName); //End record system loop time

	static void EndLoop();  //End record game loop time

	static double SystemPercentage(const char* systemName); //Convert all system loop time to percentage

	static void UpdateSystemTimes(); //Update all system loop time

private:
	ImGuiIO* io;
	ImFont* font;
	static std::unordered_map<const char*, double> systemTimes;
	static double loopStartTime;
	static double totalLoopTime;
	static double lastUpdateTime;
	static std::vector<const char*> systems;
	static std::vector<double> systemGameLoopPercent;
	static int systemCount;
};

static bool LegacyKeyDuplicationCheck(ImGuiKey key); //Prevent key duplication according to ImGui legacy key map

class CrashLog {
public:
	static void Initialise();
	static void Cleanup();
	//Log messages into crash-log.txt
	static void LogDebugMessage(const std::string& message, const char* file = nullptr, int line = 0);
	static void SignalHandler(int signum); //Log specific messages into log file according to signal receive
	static void SignalChecks(); //Check for program crash signals
	struct Exception { //Unique exception to record file and line of thrown exceptions
		std::string message;
		const char* file;
		int line;

		Exception(const std::string& msg, const char* f, int l)
			: message(msg), file(f), line(l) {}
	};
private:
	static std::ofstream logFile;

	static std::string GetCurrentTimestamp(); //Record time of logging
		
};

class GameViewWindow {
public:

	static void Initialise();

	static void Update();

private:

	static int viewportWidth;
	static int viewportHeight;
	static GLuint viewportTexture;

	static void SetupViewportTexture(); //Set up Opengl texture to store game scene

	static void CaptureMainWindow(); //Capture rendered game scene

	static void Cleanup();

	static ImVec2 GetLargestSizeForViewport(); //Resize viewport dynamically while preserving aspect ratio

	static ImVec2 GetCenteredPosForViewport(ImVec2 aspectSize); //Center viewport within available space
};

class Console {
public:

	static void Update(const char* title); 

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
		}
		return *this;
	}

private:
	static Console* instance;
    static std::vector<std::string> items;
    static bool autoScroll;
    static float lastScrollY;
	static std::ostringstream currentLog;
	Console() {}
	void DrawImpl(const char* title); //ImGui console GUI format
};