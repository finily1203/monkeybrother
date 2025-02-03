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

    static int layerToClear = -1; //can't set to 0 cause layer 0 exists
    static bool showPopup = false;

    for (int i = 0; i < layerManager.getLayerCount(); i++) {
        char label[64];
        snprintf(label, sizeof(label), "Layer %d", i);

        //Using treenode for each layer
        ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
        bool guiVisible = layerManager.getGuiVisibility(i);

        if (guiVisible) {
            nodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;
        }

        bool nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)i, nodeFlags, "%s", label);

        //Handle click to toggle visibility
        if (ImGui::IsItemClicked()) {
            layerManager.setGuiVisibility(i, !guiVisible);

            if (!GameViewWindow::getPaused()) {
                GameViewWindow::TogglePause();
            }
        }

        //If node open, display entities inside layer
        if (nodeOpen) {
            if (layerManager.getGuiVisibility(i)) {
                for (auto entity : layerManager.getEntitiesFromLayer(i)) {
                    char buttonLabel[64];
                    auto signature = ecsCoordinator.getEntityID(entity);
                    snprintf(buttonLabel, sizeof(buttonLabel), "%s", signature.c_str());

                    ImGui::PushID(entity);
                    ImGui::PushItemWidth(-1);

                    if (ImGui::Button(buttonLabel, ImVec2(-1.0f, 0.0f))) {
                        Inspector::selectedEntityID = entity;
                        Inspector::draggedEntityID = entity;
                    }

                    ImGui::PopItemWidth();
                    ImGui::PopID();
                }
            }
            char clearButton[32];
            snprintf(clearButton, sizeof(clearButton), "Clear Layer %d", i);
            if (ImGui::Button(clearButton)) {
                //pop up appear
                layerToClear = i;
				showPopup = true;
            }

            ImGui::SameLine();
			char visibilityButton[32];
			//check visibility of layer
			bool isVisible = layerManager.getLayerVisibility(i);
            if(isVisible)
			    snprintf(visibilityButton, sizeof(visibilityButton), "Disable Layer %d", i);
            else
				snprintf(visibilityButton, sizeof(visibilityButton), "Enable Layer %d", i);

			if (ImGui::Button(visibilityButton)) {
				layerManager.setLayerVisibility(i, !isVisible);
			}

            ImGui::TreePop();  // Always call TreePop when nodeOpen is true
        }
    }

	if (showPopup) {
		ImGui::OpenPopup("Clear Layer?");
	}

    if (ImGui::BeginPopupModal("Clear Layer?", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Are you sure you want to clear Layer %d?", layerToClear);
        if (ImGui::Button("Yes")) {
            if (layerManager.clearLayer(layerToClear)) {
                Console::GetLog() << "Layer " << layerToClear << " cleared" << std::endl;
            }
            else {
                Console::GetLog() << "You can't clear layer 0! || Layer trying to clear does not exist!" << std::endl;
                Console::GetLog() << "Refer to console to see exact reason" << std::endl;
            }
            layerToClear = -1;
            showPopup = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("No")) {
			layerToClear = -1;
            showPopup = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
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
