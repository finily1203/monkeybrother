#pragma once
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "../Serialization/jsonSerialization.h"
#include "../FilePaths/filePath.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h> 
#include "GlobalCoordinator.h"

#define constextptr MAXTEXTSIZE 1000

class Inspector {
public:

	static void Initialise();

	static void Update();

	static void Cleanup();

	static void RenderInspectorWindow(ECSCoordinator& ecsCoordinator, int selectedEntityID);

	static void LoadInspectorFromJSON(std::string const& filename);
private:
	static float objAttributeSliderMaxLength;
	static char textBuffer[MAXTEXTSIZE];
	static std::vector<std::pair<int, std::string>>* overlappingEntities;
};