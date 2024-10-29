/*
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Lew Zong Han Owen (z.lew)
@team   :  MonkeHood
@course :  CSD2401
@file   :  Debug.h
@brief  :  This file contains the function declaration of ImGui GUI debugging features which include the debug
		   window, game viewport window, console window, and crash logging system.

*Lew Zong Han Owen (z.lew) :
		- Integrated ImGui debug window to display FPS, performance viewer, mouse coordinates, and key/mouse input
		  indication
		- Integrated ImGui game viewport window to capture game scene in real time during debug mode
		- Integrated ImGui console window to allow direct custom debugging output in debugging mode
		- Integrated a crash logging system to detect and log custom and standard exceptions into a crash-log
		  text file

File Contributions: Lew Zong Han Owen (100%)

/*_______________________________________________________________________________________________________________*/
#pragma once
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


//Class for the ImGui GUI debugbug mode which displays debug, gameviewport, and console window
class DebugSystem : public GameSystems/*, public System*/ {
public:

	ImVec4 clear_color = ImVec4(.45f, .45f, .45f, 1.00f);
	DebugSystem();
	~DebugSystem();

	void GameSystems::initialise() override;
	//void System::initialise() override;

	//void System::update(float dt) override;
	void GameSystems::update() override;

	void GameSystems::cleanup() override;
	//void System::cleanup() override;

	SystemType getSystem() override;

	void StartLoop(); //Start record game loop time

	void StartSystemTiming(const char* systemName); //Start record system loop time

	void EndSystemTiming(const char* systemName); //End record system loop time

	void EndLoop();  //End record game loop time

	double SystemPercentage(const char* systemName); //Convert all system loop time to percentage

	void UpdateSystemTimes(); //Update all system loop time

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

	static float objSizeXMax;
	static float objSizeXMin;
	static float objSizeYMax;
	static float objSizeYMin;
};

static bool LegacyKeyDuplicationCheck(ImGuiKey key); //Prevent key duplication according to ImGui legacy key map
