#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include <GLFW/glfw3.h> 
#include "GlfwFunctions.h"
#include "Systems.h"

class DebugSystem {
public:
	ImVec4 clear_color = ImVec4(.45f, .45f, .45f, 1.00f);
	ImFont* font1;
	ImFont* font2;
	DebugSystem();
	~DebugSystem();

	void Initialise();
	void Update();
	void Cleanup();

	static void StartLoop();

	static void StartSystemTiming(const char* systemName);

	static void EndSystemTiming(const char* systemName);

	static void EndLoop();

	static double SystemPercentage(const char* systemName);

	static void UpdateSystemTimes();

private:
	ImGuiIO* io;
	static std::unordered_map<const char*, double> systemTimes;
	static double loopStartTime;
	static double totalLoopTime;
	static double lastUpdateTime;
	static std::vector<const char*> systems;
	static std::vector<double> systemGameLoopPercent;
	static int systemCount;
};

static bool LegacyKeyDuplicationCheck(ImGuiKey key);