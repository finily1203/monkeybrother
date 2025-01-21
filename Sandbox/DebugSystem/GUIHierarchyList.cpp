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
			if (ecsCoordinator.hasComponent<FontComponent>(entity)) { //TextBox specific data modification feature
				auto& fontComp = ecsCoordinator.getComponent<FontComponent>(entity);
				auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
				auto signature = ecsCoordinator.getEntityID(entity);

				size_t maxSize = 1000;
				std::string tempStr = fontComp.text;
				memset(textBuffer, 0, MAXTEXTSIZE);
				size_t length = std::min(tempStr.length(), maxSize);
				for (size_t i = 0; i < length; i++)
				{
					textBuffer[i] = tempStr[i];
				}
				ImGui::PushID(entity);
			;
				if (ImGui::TreeNode("Signature: %s", signature.c_str())) {
					
					//Inspector::selectedEntityID = entity;
					
					float colour[3] = { fontComp.color.GetX(),fontComp.color.GetY(),fontComp.color.GetZ() };
					if (ImGui::DragFloat3("Colour", colour, 0.01f, 0.0f, 1.0f)) {
						fontComp.color.SetX(colour[0]);
						fontComp.color.SetY(colour[1]);
						fontComp.color.SetZ(colour[2]);
					}

					float pos[2] = { transform.position.GetX(), transform.position.GetY() };
					if (ImGui::DragFloat2("Position", pos, 5.f)) {
						transform.position.SetX(pos[0]);
						transform.position.SetY(pos[1]);
					}

					float scale[1] = { fontComp.textScale };
					if (ImGui::DragFloat("Scale", scale, 0.5f)) {
						fontComp.textScale = scale[0];
					}

					float rotation[1] = { transform.orientation.GetX() };
					if (ImGui::DragFloat("Rotation", rotation, 1.f)) {
						transform.orientation.SetX(rotation[0]);
					}

					float textBox[1] = { fontComp.textBoxWidth };
					if (ImGui::DragFloat("TextBox", textBox, 1.f)) {
						fontComp.textBoxWidth = textBox[0];
					}

					ImGui::SetNextItemWidth(objAttributeSliderMaxLength);  // Set width of input field
					ImGui::InputText("##Text", textBuffer, IM_ARRAYSIZE(textBuffer));
					std::cout << textBuffer << std::endl;
					ImGui::SameLine();
					ImGui::Text("Text");
					fontComp.text = textBuffer;

					if (ImGui::Button("Remove")) {
						ecsCoordinator.destroyEntity(entity);
					}

					ImGui::TreePop();

				}
				
				ImGui::PopID();
				ImGui::Separator();

			}
			else if (ecsCoordinator.hasComponent<PhysicsComponent>(entity) //Player specific data modification features
				&& !ecsCoordinator.hasComponent<EnemyComponent>(entity)) {
				auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
				auto signature = ecsCoordinator.getEntityID(entity);
				
				ImGui::PushID(entity);

				if (ImGui::TreeNode("Signature: %s", signature.c_str())) {
					
					//Inspector::selectedEntityID = entity;
					
					float pos[2] = { transform.position.GetX(), transform.position.GetY() };
					if (ImGui::DragFloat2("Position", pos, 5.f)) {
						transform.position.SetX(pos[0]);
						transform.position.SetY(pos[1]);
					}

					float scale[1] = { transform.scale.GetX() };
					if (ImGui::DragFloat("Scale", scale, 1.f)) {
						transform.scale.SetX(scale[0]);
						transform.scale.SetY(scale[0]);
					}

					float rotation[1] = { transform.orientation.GetX() };
					if (ImGui::DragFloat("Rotation", rotation, 1.f)) {
						transform.orientation.SetX(rotation[0]);
					}

					if (ImGui::Button("Remove")) {
						ecsCoordinator.destroyEntity(entity);
					}

					ImGui::TreePop();

				}
				

				ImGui::PopID();
				ImGui::Separator();
			}
			else
			{ //Remaining object's data modification features
				auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
				auto signature = ecsCoordinator.getEntityID(entity);
				
				ImGui::PushID(entity);

				if (ImGui::TreeNode("Signature: %s", signature.c_str())) {
					
					//Inspector::selectedEntityID = entity;
					
					float pos[2] = { transform.position.GetX(), transform.position.GetY() };
					if (ImGui::DragFloat2("Position", pos, 5.f)) {
						transform.position.SetX(pos[0]);
						transform.position.SetY(pos[1]);
					}

					float scale[2] = { transform.scale.GetX(), transform.scale.GetY() };
					if (ImGui::DragFloat2("Scale", scale, 1.f)) {
						transform.scale.SetX(scale[0]);
						transform.scale.SetY(scale[1]);
					}

					float rotation[1] = { transform.orientation.GetX() };
					if (ImGui::DragFloat("Rotation", rotation, 1.f)) {
						transform.orientation.SetX(rotation[0]);
					}

					if (ImGui::Button("Remove")) {
						ecsCoordinator.destroyEntity(entity);
					}

					ImGui::TreePop();

				}
				

				ImGui::PopID();
				ImGui::Separator();

			}
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
