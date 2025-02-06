/*
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author   :  Lew Zong Han Owen (z.lew)
@coauthor : Chu Wei Yuan Joel (c.weiyuan)
@team     :  MonkeHood
@course   :  CSD2401
@file     :  GUIHierarchyList.cpp
@brief    :  This file contains the function definition of ImGui hierarchy list system

*Lew Zong Han Owen (z.lew) :
		- Integrated ImGui hierarchy list to display all existing entities. Allow individual entity editing

*Chu Wei Yuan Joel (c.weiyuan):
		- Integrated layering system and its controls into the hierarchy list panel

File Contributions: Lew Zong Han Owen (70%)
                    Chu Wei Yuan Joel (30%)

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

    static int layerToClear = -1; // -1 because layer 0 is a layer
    static bool showPopup = false;
    static int entityDraggedFrom = -1; // -1 because layer 0 is a layer

    for (int i = 0; i < layerManager.getLayerCount(); i++) {
        char label[64];
        snprintf(label, sizeof(label), "Layer %d", i);

        ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
        bool guiVisible = layerManager.getGuiVisibility(i);

        if (guiVisible) {
            nodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;
        }

        bool nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)i, nodeFlags, "%s", label);

        // Toggle layer visibility on click
        if (ImGui::IsItemClicked()) {
            layerManager.setGuiVisibility(i, !guiVisible);
            if (!GameViewWindow::getPaused()) {
                GameViewWindow::TogglePause();
            }
        }

		//Drop entity to layer
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_DRAG")) {
                IM_ASSERT(payload->DataSize == sizeof(int));
                int draggedEntity = *(const int*)payload->Data;

                // Move entity to the current layer
                layerManager.shiftEntityToLayer(entityDraggedFrom, i, draggedEntity);
                Console::GetLog() << "Entity moved to Layer " << i << std::endl;
            }
            ImGui::EndDragDropTarget();
        }

        if (nodeOpen) {
            if (guiVisible) {
                for (auto entity : layerManager.getEntitiesFromLayer(i)) {
                    char buttonLabel[64];
                    auto signature = ecsCoordinator.getEntityID(entity);
                    snprintf(buttonLabel, sizeof(buttonLabel), "%s", signature.c_str());

                    ImGui::PushID(entity);
                    ImGui::PushItemWidth(-1);

                    // Entity button
                    if (ImGui::Button(buttonLabel, ImVec2(-1.0f, 0.0f))) {
                        Inspector::selectEntityID = entity;
                        Inspector::draggedEntityID = entity;
                    }

                    //Drag entity
                    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                        ImGui::SetDragDropPayload("ENTITY_DRAG", &entity, sizeof(int)); // Send entity ID
                        ImGui::Text("Moving %s", buttonLabel); // Drag label
                        ImGui::EndDragDropSource();
						entityDraggedFrom = i;
                    }

                    ImGui::PopItemWidth();
                    ImGui::PopID();
                }
            }

            // Clear Layer Button
            char clearButton[32];
            snprintf(clearButton, sizeof(clearButton), "Clear Layer %d", i);
            if (ImGui::Button(clearButton)) {
                layerToClear = i;
                showPopup = true;
            }

            ImGui::SameLine();
            char visibilityButton[32];
            bool isVisible = layerManager.getLayerVisibility(i);
            snprintf(visibilityButton, sizeof(visibilityButton), isVisible ? "Disable Layer %d" : "Enable Layer %d", i);

            if (ImGui::Button(visibilityButton)) {
                layerManager.setLayerVisibility(i, !isVisible);
            }

            ImGui::TreePop();
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
                Console::GetLog() << "Cannot clear layer 0 or invalid layer!" << std::endl;
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
