#include "GUIInspector.h"
#include "GUIGameViewport.h"
#include "Debug.h"
#include "FontSystemECS.h"

float Inspector::objAttributeSliderMaxLength;
char Inspector::textBuffer[MAXTEXTSIZE];
ImVec2 mouseWorldPos;
glm::mat4 projectionMatrix(1.0f);

void Inspector::Initialise() {
	LoadInspectorFromJSON(FilePathManager::GetIMGUIInspectorJSONPath());
}

void Inspector::Update() {

	mouseWorldPos = GameViewWindow::GetMouseWorldPosition();

	ImVec2 viewportPos = GameViewWindow::getViewportPos(); // Get viewport's position
	ImVec2 mouseScreenPos = ImGui::GetMousePos();
	//std::cout << "Mouse Screen: " << mouseScreenPos.x << ", " << mouseScreenPos.y << std::endl;
	mouseScreenPos.x -= viewportPos.x;  // Adjust for viewport offset
	mouseScreenPos.y -= viewportPos.y;

	mouseScreenPos.x = (2.0f * mouseScreenPos.x / GLFWFunctions::windowWidth) - 1.0f;
	mouseScreenPos.y = 1.0f - (2.0f * mouseScreenPos.y / GLFWFunctions::windowHeight);

	static int selectedEntityID = -1; // -1 means no entity is selected
	static int draggedEntityID = -1;  // -1 means no entity is being dragged
	static int chosenEntityID = -1;   // Entity selected for potential deletion
	bool openDeletePopup = false;

	Console::GetLog() << mouseWorldPos.x << "," << mouseWorldPos.y << std::endl;

	projectionMatrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
	ImVec2 centeredMouse = GameViewWindow::GetCenteredMousePosition();
	Console::GetLog() << "Mouse Relative: X: " << centeredMouse.x << " Y: " << centeredMouse.y << std::endl;
	auto isMouseOverEntity = [&](int entity, float& distanceSquared, bool useCircular = false) -> bool {
		if (ecsCoordinator.hasComponent<FontComponent>(entity) &&
			ecsCoordinator.hasComponent<TransformComponent>(entity)) {
			const auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);

			float x = transform.position.GetX();
			float y = transform.position.GetY();
			float width = 400.0f;  // Increased from 200 to 400
			float height = 100.0f;  // Increased from 50 to 100

			// Center the collision box horizontally relative to the position
			float leftBound = x;
			float rightBound = x + (width * 0.4f);
			float topBound = y + height * 0.6f;    // Increased upper bound
			float bottomBound = y - height * 0.3f;  // Lowered bottom bound

			// Calculate distance for circular collision from centered position
			float centerX = x;  // Use position directly since bounds are centered
			float centerY = bottomBound + height * 0.5f;
			float dx = centeredMouse.x - centerX;
			float dy = centeredMouse.y - centerY;
			distanceSquared = dx * dx + dy * dy;

			if (useCircular) {
				float normalizedX = dx / (width * 0.5f);
				float normalizedY = dy / (height * 0.5f);
				return (normalizedX * normalizedX + normalizedY * normalizedY) <= 1.0f;
			}
			else {
				return (centeredMouse.x >= leftBound && centeredMouse.x <= rightBound &&
					centeredMouse.y <= topBound && centeredMouse.y >= bottomBound);
			}
		}
		else if (ecsCoordinator.hasComponent<TransformComponent>(entity)) {
			// Keep your existing non-text entity collision code
			const auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);
			float halfWidth = std::abs(transform.scale.GetX() * 0.5f);
			float halfHeight = std::abs(transform.scale.GetY() * 0.5f);
			float dx = mouseWorldPos.x - transform.position.GetX();
			float dy = mouseWorldPos.y - transform.position.GetY();
			distanceSquared = dx * dx + dy * dy;

			if (useCircular) {
				float normalizedX = dx / halfWidth;
				float normalizedY = dy / halfHeight;
				return (normalizedX * normalizedX + normalizedY * normalizedY) <= 1.0f;
			}
			else {
				return (std::abs(dx) <= halfWidth && std::abs(dy) <= halfHeight);
			}
		}
		return false;
		};

	Console::GetLog() << mouseWorldPos.x << "," << mouseWorldPos.y << std::endl;

	std::string selEntityID; // Store selected entity ID instead of Entity handle
	static std::vector<std::pair<int, std::string>> overlappingEntities;

	static bool isSelectingEntity = false;
	static bool isInitialClickAfterSelection = true;

	static ImVec2 mousePos;
	mousePos = ImGui::GetMousePos();

	static bool initiatedByDoubleClick = false;
	//static bool openDeletePopup = false;
	//static int chosenEntityID = -1;

	// Handle entity selection with Ctrl+Left Click
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && GameViewWindow::IsPointInViewport(mousePos.x, mousePos.y)) {
		selectedEntityID = -1;
		isSelectingEntity = true;
		isInitialClickAfterSelection = true;
		overlappingEntities.clear();

		// Get all entities and check for collision
		for (auto entity : ecsCoordinator.getAllLiveEntities()) {
			if (ecsCoordinator.getEntityID(entity) != "placeholderentity") {
				float distSq;
				if (isMouseOverEntity(entity, distSq)) {
					overlappingEntities.push_back({ entity, ecsCoordinator.getEntityID(entity) });
				}
			}
		}

		// Sort overlapping entities by distance if needed
		if (overlappingEntities.size() > 1) {
			std::sort(overlappingEntities.begin(), overlappingEntities.end(),
				[](const auto& a, const auto& b) {
					return a.second < b.second;
				});
		}

		if (!overlappingEntities.empty()) {
			ImGui::OpenPopup("Select Entity");
		}
		else {
			selectedEntityID = -1;
			draggedEntityID = -1;
			isSelectingEntity = false;
		}
	}

	// Entity Selection Popup
	if (ImGui::BeginPopup("Select Entity")) {
		if (ImGui::IsWindowAppearing()) {
			ImGui::SetWindowPos(ImGui::GetMousePos());
		}
		ImGui::Text("Select Entity:");
		ImGui::Separator();

		for (const auto& [entity, name] : overlappingEntities) {
			if (ImGui::MenuItem(name.c_str())) {
				selectedEntityID = entity;
				draggedEntityID = entity;
				isSelectingEntity = false;
				isInitialClickAfterSelection = true;
				initiatedByDoubleClick = false;  // Reset double-click state on new selection
				ImGui::CloseCurrentPopup();
				break;
			}
		}
		ImGui::EndPopup();
	}

	// Handle dragging with double-click
	if (draggedEntityID != -1 && !isSelectingEntity) {
		if (isInitialClickAfterSelection || !initiatedByDoubleClick) {
			if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				isInitialClickAfterSelection = false;
				initiatedByDoubleClick = true;
			}
		}
		else if (initiatedByDoubleClick && ImGui::IsMouseDown(ImGuiMouseButton_Left) &&
			GameViewWindow::IsPointInViewport(mousePos.x, mousePos.y)) {
			auto& transform = ecsCoordinator.getComponent<TransformComponent>(draggedEntityID);
			if (ecsCoordinator.hasComponent<FontComponent>(draggedEntityID)) {
				transform.position.SetX(centeredMouse.x);
				transform.position.SetY(centeredMouse.y);
			}
			else {
				transform.position.SetX(mouseWorldPos.x);
				transform.position.SetY(mouseWorldPos.y);
			}
		}
		else if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
			initiatedByDoubleClick = false;
		}
	}

	// Handle mouse wheel scaling for selected entity
	if (selectedEntityID != -1 && ImGui::GetIO().MouseWheel != 0.0f) {
		float wheel_delta = ImGui::GetIO().MouseWheel;
		auto& transform = ecsCoordinator.getComponent<TransformComponent>(selectedEntityID);
		float dx = mouseWorldPos.x - transform.position.GetX();
		float dy = mouseWorldPos.y - transform.position.GetY();
		float halfWidth = std::abs(transform.scale.GetX() / 2.0f);
		float halfHeight = std::abs(transform.scale.GetY() / 2.0f);

		// Check if mouse is within bounds, accounting for any combination of negative/positive scales
		bool withinX = std::abs(dx) <= halfWidth;
		bool withinY = std::abs(dy) <= halfHeight;
		float scaleFactor = 1.0f + (wheel_delta * 0.1f);

		if (withinX && withinY) {
			if (ecsCoordinator.getEntityID(selectedEntityID) != "player") {
				if (ImGui::GetIO().KeyShift) {
					float rotationDelta = wheel_delta * 15.0f;
					transform.orientation.SetX(transform.orientation.GetX() + rotationDelta);
				}
				else if (ImGui::GetIO().KeyCtrl) {
					// Preserve the sign when scaling X
					float newScaleX = transform.scale.GetX() * scaleFactor;
					transform.scale.SetX(newScaleX);
				}
				else {
					// Preserve the sign when scaling Y
					float newScaleY = transform.scale.GetY() * scaleFactor;
					transform.scale.SetY(newScaleY);
				}
			}
			else {
				if (ImGui::GetIO().KeyShift) {
					float rotationDelta = wheel_delta * 15.0f;
					transform.orientation.SetX(transform.orientation.GetX() + rotationDelta);
				}
				else if (ImGui::GetIO().KeyCtrl) {
					// Preserve the sign when scaling X
					float newScaleX = transform.scale.GetX() * scaleFactor;
					transform.scale.SetX(newScaleX);
					transform.scale.SetY(newScaleX);
				}
				else {
					// Preserve the sign when scaling Y
					float newScaleY = transform.scale.GetX() * scaleFactor;
					transform.scale.SetX(newScaleY);
					transform.scale.SetY(newScaleY);
				}
			}
			
		}
	}

	// Handle right-click deletion for selected entity
	if (ImGui::IsKeyPressed(ImGuiKey_Delete) && selectedEntityID != -1) {
		float distSq;
		if (isMouseOverEntity(selectedEntityID, distSq)) {
			openDeletePopup = true;
		}
	}

	if (openDeletePopup) {
		ImGui::OpenPopup("Delete Entity?");
		openDeletePopup = false;
	}

	ImVec2 middle = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(middle, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Delete Entity?", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("Are you sure you want to delete this entity?");
		ImGui::Separator();

		if (ImGui::Button("Delete", ImVec2(120, 0))) {
			ecsCoordinator.destroyEntity(selectedEntityID);
			selectedEntityID = -1;
			draggedEntityID = -1;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) {
			selectedEntityID = -1;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	// Handle selection mode cancellation
	if (isSelectingEntity && !ImGui::IsPopupOpen("Select Entity")) {
		isSelectingEntity = false;
	}

	RenderInspectorWindow(ecsCoordinator, selectedEntityID);
}

void Inspector::RenderInspectorWindow(ECSCoordinator& ecs, int selectedEntityID) {
	ImGui::Begin("Inspector");

	if (selectedEntityID == -1) {
		ImGui::TextDisabled("No entity selected");
		ImGui::End();
		return;
	}

	// Entity ID/Name display
	std::string entityID = ecs.getEntityID(selectedEntityID);
	ImGui::Text("Entity ID: %s", entityID.c_str());
	ImGui::Separator();

	if (ecsCoordinator.getEntityID(selectedEntityID) == "placeholderentity") {}
	else
		if (ecsCoordinator.hasComponent<FontComponent>(selectedEntityID)) { //TextBox specific data modification feature
			auto& fontComp = ecsCoordinator.getComponent<FontComponent>(selectedEntityID);
			auto& transform = ecsCoordinator.getComponent<TransformComponent>(selectedEntityID);
			auto signature = ecsCoordinator.getEntityID(selectedEntityID);

			memset(textBuffer, 0, MAXTEXTSIZE);
			size_t maxSize = 1000;
			std::string tempStr = fontComp.text;

			size_t length = std::min(tempStr.length(), maxSize);
			for (size_t i = 0; i < length; i++)
			{
				textBuffer[i] = tempStr[i];
			}

			ImGui::PushID(selectedEntityID);



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
			ImGui::SameLine();
			ImGui::Text("Text");
			fontComp.text = textBuffer;

			if (ImGui::Button("Remove")) {
				ecsCoordinator.destroyEntity(selectedEntityID);
			}

			ImGui::PopID();
			ImGui::Separator();

		}
		else if (ecsCoordinator.hasComponent<PhysicsComponent>(selectedEntityID) //Player specific data modification features
			&& !ecsCoordinator.hasComponent<EnemyComponent>(selectedEntityID)) {
			auto& transform = ecsCoordinator.getComponent<TransformComponent>(selectedEntityID);
			auto signature = ecsCoordinator.getEntityID(selectedEntityID);

			ImGui::PushID(selectedEntityID);


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
				ecsCoordinator.destroyEntity(selectedEntityID);
			}

			ImGui::PopID();
			ImGui::Separator();
		}
		else
		{ //Remaining object's data modification features
			auto& transform = ecsCoordinator.getComponent<TransformComponent>(selectedEntityID);
			auto signature = ecsCoordinator.getEntityID(selectedEntityID);

			ImGui::PushID(selectedEntityID);


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
				ecsCoordinator.destroyEntity(selectedEntityID);
			}


			ImGui::PopID();
			ImGui::Separator();

		}

	ImGui::End();
}

void Inspector::LoadInspectorFromJSON(std::string const& filename)
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

	serializer.ReadCharArray(textBuffer, MAXTEXTSIZE, "Inspector.textBuffer");
	serializer.ReadFloat(objAttributeSliderMaxLength, "Inspector.objAttributeSliderMaxLength");
}