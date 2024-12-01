/*
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Lew Zong Han Owen (z.lew)
@team   :  MonkeHood
@course :  CSD2401
@file   :  Debug.cpp
@brief  :  This file contains the function declaration of ImGui main GUI debugging window and it also coordinates the
		   the other ImGui sub systems' such as game viewport, console, crash logging, game viewport, object creation,
		   hierarchy list, and save and load feature, asset browser, and inspector window

*Lew Zong Han Owen (z.lew) :
		- Integrated ImGui debug window to display FPS, performance viewer, mouse coordinates, and key/mouse input
		  indication
		- Designed the display synergy between all of ImGui's sub systems in the main debugging window

*Ian Loi (ian.loi) :
		- Integrated serialization & deserialization functions to initialize variables from json file, which allows
		  saving and loading feature in the level editor

File Contributions: Lew Zong Han Owen (80%)
					Ian Loi           (20%)

/*_______________________________________________________________________________________________________________*/
#pragma once
#include "EngineDefinitions.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h> 
#include "GlfwFunctions.h"

#include "ECSCoordinator.h"
#include "Systems.h"
#include "matrix3x3.h"
#include "GraphicsSystem.h"
#include "WindowSystem.h"
#include "SystemManager.h"
#include "TransformComponent.h"
#include "../Serialization/jsonSerialization.h"

#define MAXNAMELENGTH 15
#define MAXBUFFERSIZE 8
#define MAXTEXTSIZE 1000
#define MAXOBJTYPE 4

//Class for the ImGui GUI debugbug mode which displays debug, gameviewport, and console window
class DebugSystem : public GameSystems/*, public System*/ {
public:

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

	void UpdateSystemTimes(); //Update all system loop time

	//Load debug system configuration from JSON file
	void LoadDebugConfigFromJSON(std::string const& filename);

	static std::vector<Entity>* newEntities;

	static ImVec2 getMouseWorldPos() {
		return mouseWorldPos;
	}
private:
	char* iniPath;
	ImGuiIO* io;
	ImFont* font;
	float fontSize;
	float textBorderSize;
	ImVec4 clearColor;
	float displayBuffer;
	float paddingPV;
	double ecsTotal;
	bool foundECS;
	std::unordered_map<std::string, double>* systemStartTimes;
	std::unordered_map<std::string, double>* accumulatedTimes;
	double loopStartTime;
	double totalLoopTime;
	double lastUpdateTime;
	bool firstFrame;
	std::vector<const char*>* systems;
	std::vector<double>* systemGameLoopPercent;
	int systemCount;
	static ImVec2 mouseWorldPos;
};

static bool LegacyKeyDuplicationCheck(ImGuiKey key); //Prevent key duplication according to ImGui legacy key map