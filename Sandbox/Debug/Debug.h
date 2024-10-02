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

	static void ToggleAllWindows(bool show);

	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

	static void StartLoop();

	static void StartSystemTiming(const char* systemName);

	static void EndSystemTiming(const char* systemName);

	static void EndLoop();

	static double SystemPercentage(const char* systemName);

	static void UpdateSystemTimes();

private:
	ImGuiIO* io;
	ImFont* font1;
	ImFont* font2;
	static bool showGUI;
	static bool checkFrame;
	static std::unordered_map<const char*, double> systemTimes;
	static double loopStartTime;
	static double totalLoopTime;
	static double lastUpdateTime;
	static std::vector<const char*> systems;
	static std::vector<double> systemGameLoopPercent;
	static int systemCount;
};

static bool LegacyKeyDuplicationCheck(ImGuiKey key);

class CrashLog {
public:
	static void Initialise();
	static void Cleanup();
	static void LogDebugMessage(const std::string& message, const char* file = nullptr, int line = 0);
	static void SignalHandler(int signum);
	static void SignalChecks();
	struct Exception {
		std::string message;
		const char* file;
		int line;

		Exception(const std::string& msg, const char* f, int l)
			: message(msg), file(f), line(l) {}
	};
private:
	static std::ofstream logFile;

	static std::string getCurrentTimestamp() {
		std::time_t now = std::time(nullptr);
        std::tm timeinfo;
        localtime_s(&timeinfo, &now);
        std::ostringstream oss;
        oss << std::put_time(&timeinfo, "%Y-%m-%d %H:%M:%S");
        return oss.str();
	}
};

class GameViewWindow {
public:

	static void Initialise();

	static void Update();


private:

	static int viewportWidth;
	static int viewportHeight;
	static GLuint viewportTexture;

	static void SetupViewportTexture();

	static void CaptureMainWindow();
	static void Cleanup();

	static ImVec2 GetLargestSizeForViewport();

	static ImVec2 GetCenteredPosForViewport(ImVec2 aspectSize);
};