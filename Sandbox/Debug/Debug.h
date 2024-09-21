#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include <GLFW/glfw3.h> 
#include "GlfwFunctions.h"

class Debug {
public:
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	//ImFont* font1;
	//ImFont* font2;
	std::vector<const char*> systems = { "Collision", "Physics", "Graphics" };
	Debug();
	~Debug();
	
	float get_deltaTime() { return ImGui::GetIO().DeltaTime; }

	void Run(double& fps);

	void Display();

private:
	ImGuiIO* io;
	std::unordered_map<std::string, double> systemTimes;
	double loopStartTime;
	double totalLoopTime;
};

static bool LegacyKeyDuplicationCheck(ImGuiKey key);