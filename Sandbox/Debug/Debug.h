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

class DebugSystem : public Systems {
public:
	ImVec4 clear_color = ImVec4(.45f, .45f, .45f, 1.00f);
	//ImFont* font1;
	//ImFont* font2;
	std::vector<const char*> systems = { "Collision", "Physics", "Graphics" };
	DebugSystem();
	~DebugSystem();

	void initialise() override;
	void update() override;
	void cleanup() override;

	void startLoop();

	void startSystemTiming(const std::string& systemName);

	void endSystemTiming(const std::string& systemName);

	void endLoop();
	//void Run(double& fps);

	//void Display();

private:
	ImGuiIO* io;
	std::unordered_map<std::string, double> systemTimes;
	double loopStartTime;
	double totalLoopTime;
};
//std::unordered_map<std::string, double> systemTimes;
	//double loopStartTime;
	//double totalLoopTime;
static bool LegacyKeyDuplicationCheck(ImGuiKey key);