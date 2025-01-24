/*
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Lew Zong Han Owen (z.lew)
@team   :  MonkeHood
@course :  CSD2401
@file   :  GUIHierarchyList.cpp
@brief  :  This file contains the function definition of ImGui hierarchy list system

*Lew Zong Han Owen (z.lew) :
		- Integrated ImGui hierarchy list to display all existing entities. Allow individual entity editing

File Contributions: Lew Zong Han Owen (100%)

/*_______________________________________________________________________________________________________________*/
#include "GUIHierarchyList.h"
#include "GUIInspector.h"
#include "GUIGameViewport.h"

float HierarchyList::objAttributeSliderMaxLength;
char HierarchyList::textBuffer[MAXTEXTSIZE];

void HierarchyList::Initialise() {
	LoadHierarchyListFromJSON(FilePathManager::GetIMGUIHierarchyJSONPath());
}

void HierarchyList::Update() {
	ImGui::Begin("Hierarchy List");

	for (auto entity : ecsCoordinator.getAllLiveEntities()) {
		if (ecsCoordinator.getEntityID(entity) == "placeholderentity") {
		}
		else
		{
			auto signature = ecsCoordinator.getEntityID(entity);

			ImGui::PushID(entity);

			char buttonLabel[64];
			snprintf(buttonLabel, sizeof(buttonLabel), "%s", signature.c_str());
			ImGui::PushItemWidth(-1);
			if (ImGui::Button(buttonLabel, ImVec2(-1.0f, 0.0f))) {
				if (!GameViewWindow::getPaused()) {
					GameViewWindow::TogglePause();
				}
				Inspector::selectedEntityID = entity;
				Inspector::draggedEntityID = entity;
			}
			ImGui::PopItemWidth();

			ImGui::PopID();
		}
		//if (ecsCoordinator.hasComponent<FontComponent>(entity)) { //TextBox specific data modification feature
		//	auto& fontComp = ecsCoordinator.getComponent<FontComponent>(entity);
		//	auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
		//	auto signature = ecsCoordinator.getEntityID(entity);

		//	size_t maxSize = 1000;
		//	std::string tempStr = fontComp.text;
		//	memset(textBuffer, 0, MAXTEXTSIZE);
		//	size_t length = std::min(tempStr.length(), maxSize);
		//	for (size_t i = 0; i < length; i++)
		//	{
		//		textBuffer[i] = tempStr[i];
		//	}
		//	ImGui::PushID(entity);

		//	char buttonLabel[64];
		//	snprintf(buttonLabel, sizeof(buttonLabel), "%s", signature.c_str());
		//	ImGui::PushItemWidth(-1);
		//	if (ImGui::Button(buttonLabel, ImVec2(-1.0f, 0.0f))) {
		//		Inspector::selectedEntityID = entity;
		//	}
		//	ImGui::PopItemWidth();

		//	ImGui::PopID();

		//}
		//else if (ecsCoordinator.hasComponent<PhysicsComponent>(entity) //Player specific data modification features
		//	&& !ecsCoordinator.hasComponent<EnemyComponent>(entity)) {
		//	auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
		//	auto signature = ecsCoordinator.getEntityID(entity);
		//	
		//	ImGui::PushID(entity);

		//	char buttonLabel[64];
		//	snprintf(buttonLabel, sizeof(buttonLabel), "%s", signature.c_str());
		//	ImGui::PushItemWidth(-1);
		//	if (ImGui::Button(buttonLabel, ImVec2(-1.0f, 0.0f))) {
		//		Inspector::selectedEntityID = entity;
		//	}
		//	ImGui::PopItemWidth();

		//	ImGui::PopID();
		//}
		//else
		//{ //Remaining object's data modification features
		//	auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
		//	auto signature = ecsCoordinator.getEntityID(entity);

		//	ImGui::PushID(entity);

		//	char buttonLabel[64];
		//	snprintf(buttonLabel, sizeof(buttonLabel), "%s", signature.c_str());
		//	ImGui::PushItemWidth(-1); 
		//	if (ImGui::Button(buttonLabel, ImVec2(-1.0f, 0.0f))) {  
		//		Inspector::selectedEntityID = entity;
		//	}
		//	ImGui::PopItemWidth();

		//	ImGui::PopID();

		//}
	}


	ImGui::End();
}

void HierarchyList::LoadHierarchyListFromJSON(std::string const& filename)
{
	JSONSerializer serializer;

	// checks if the JSON file can be opened
	if (!serializer.Open(filename))
	{
		Console::GetLog() << "Error: could not open file " << filename << std::endl;
		return;
	}

	// retrieve the JSON object from the JSON file
	nlohmann::json currentObj = serializer.GetJSONObject();

	serializer.ReadCharArray(textBuffer, MAXTEXTSIZE, "Hierarchy.textBuffer");
	serializer.ReadFloat(objAttributeSliderMaxLength, "Hierarchy.objAttributeSliderMaxLength");
}
void HierarchyList::Cleanup() {

}
