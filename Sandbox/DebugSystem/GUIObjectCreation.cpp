#include "Debug.h"
#include "GUIObjectCreation.h"

int ObjectCreation::objCount;
float ObjectCreation::objAttributeSliderMaxLength;
float ObjectCreation::objAttributeSliderMidLength;
float ObjectCreation::objAttributeSliderMinLength;
int ObjectCreation::currentItem;
char ObjectCreation::sigBuffer[MAXNAMELENGTH];
char ObjectCreation::textBuffer[MAXTEXTSIZE];
char ObjectCreation::textScaleBuffer[MAXBUFFERSIZE];
char ObjectCreation::xCoordinatesBuffer[MAXBUFFERSIZE];
char ObjectCreation::yCoordinatesBuffer[MAXBUFFERSIZE];
char ObjectCreation::xOrientationBuffer[MAXBUFFERSIZE];
char ObjectCreation::yOrientationBuffer[MAXBUFFERSIZE];
float ObjectCreation::xCoordinates;
float ObjectCreation::yCoordinates;
float ObjectCreation::xOrientation;
float ObjectCreation::yOrientation;
float ObjectCreation::textScale;
float ObjectCreation::textBoxWidth;
float ObjectCreation::textScaleMinLimit;
float ObjectCreation::coordinateMaxLimitsX;
float ObjectCreation::coordinateMaxLimitsY;
float ObjectCreation::coordinateMinLimitsX;
float ObjectCreation::coordinateMinLimitsY;
float ObjectCreation::orientationMaxLimit;
float ObjectCreation::orientationMinLimit;
int ObjectCreation::numEntitiesToCreate;
int ObjectCreation::minEntitesToCreate;

char ObjectCreation::numBuffer[MAXBUFFERSIZE];
float ObjectCreation::defaultObjScaleX;
float ObjectCreation::defaultObjScaleY;
float ObjectCreation::textBoxMaxLimit;
float ObjectCreation::textBoxMinLimit;

void ObjectCreation::Initialise() {
	LoadObjectCreationFromJSON(FilePathManager::GetIMGUICreationJSONPath());
}

void ObjectCreation::Update() {
	ImGui::Begin("Object Creation");

	ImGui::AlignTextToFramePadding();

	ImGui::Text("Total no. of objects: %d", objCount);

	objCount = 0;

	objCount += static_cast<int>(ecsCoordinator.getAllLiveEntities().size()) - 1;

	const char* items[] = { "Player", "Enemy", "Platform", "TextBox", "Background" };

	// Create the combo box
	ImGui::SetNextItemWidth(objAttributeSliderMidLength);
	if (ImGui::BeginCombo("##dropdown", items[currentItem])) {
		for (int i = 0; i < IM_ARRAYSIZE(items); i++) {
			const bool typeSelected = (currentItem == i);
			if (ImGui::Selectable(items[i], typeSelected)) {
				currentItem = i;
			}
		}
		ImGui::EndCombo();
	}
	ImGui::SameLine();
	ImGui::Text("Object type");

	if (!strcmp(items[currentItem], "TextBox")) { //Specifically for textbox
		ImGui::SetNextItemWidth(objAttributeSliderMidLength);  // Set width of input field
		ImGui::InputText("##Signature", sigBuffer, IM_ARRAYSIZE(sigBuffer));
		ImGui::SameLine();
		ImGui::Text("Name");

		ImGui::SetNextItemWidth(objAttributeSliderMaxLength);  // Set width of input field
		ImGui::InputText("##Text", textBuffer, IM_ARRAYSIZE(textBuffer));
		ImGui::SameLine();
		ImGui::Text("Text");

		ImGui::SetNextItemWidth(objAttributeSliderMinLength);  // Set width of input field
		ImGui::InputText("##TextScale", textScaleBuffer, IM_ARRAYSIZE(textScaleBuffer));
		ImGui::SameLine();
		ImGui::Text("Scale");

		textScale = std::max(textScaleMinLimit, strtof(textScaleBuffer, nullptr));

		ImGui::SetNextItemWidth(objAttributeSliderMinLength);
		ImGui::InputText("##X Pos", xCoordinatesBuffer, IM_ARRAYSIZE(xCoordinatesBuffer));

		ImGui::SameLine();
		ImGui::Text(",");

		ImGui::SameLine();
		ImGui::SetNextItemWidth(objAttributeSliderMinLength);
		ImGui::InputText("##Y Pos", yCoordinatesBuffer, IM_ARRAYSIZE(yCoordinatesBuffer));

		ImGui::SameLine();
		ImGui::Text("Coordinates");
	}
	else { //The remaining object types
		ImGui::SetNextItemWidth(objAttributeSliderMidLength);  // Set width of input field
		ImGui::InputText("##Signature", sigBuffer, IM_ARRAYSIZE(sigBuffer));
		ImGui::SameLine();
		ImGui::Text("Name");

		ImGui::SetNextItemWidth(objAttributeSliderMidLength);  // Set width of input field
		ImGui::InputText("##count", numBuffer, IM_ARRAYSIZE(numBuffer), ImGuiInputTextFlags_CharsDecimal);
		ImGui::SameLine();
		ImGui::Text("Objects to be created");

		numEntitiesToCreate = std::max(minEntitesToCreate, atoi(numBuffer));

		ImGui::SetNextItemWidth(objAttributeSliderMinLength);
		ImGui::InputText("##X Pos", xCoordinatesBuffer, IM_ARRAYSIZE(xCoordinatesBuffer));

		ImGui::SameLine();
		ImGui::Text(",");

		ImGui::SameLine();
		ImGui::SetNextItemWidth(objAttributeSliderMinLength);
		ImGui::InputText("##Y Pos", yCoordinatesBuffer, IM_ARRAYSIZE(yCoordinatesBuffer));

		ImGui::SameLine();
		ImGui::Text("Coordinates");
	}

	xCoordinates = std::max(coordinateMinLimitsX, strtof(xCoordinatesBuffer, nullptr));
	yCoordinates = std::max(coordinateMinLimitsY, strtof(yCoordinatesBuffer, nullptr));

	Entity entityObj;
	std::string entityId;

	if (ImGui::Button("Create Entity")) {

		for (int i = 0; i < numEntitiesToCreate; i++) {

			entityObj = ecsCoordinator.createEntity();

			entityId = sigBuffer;

			TransformComponent transform{};

			transform.position.SetX(xCoordinates);
			transform.position.SetY(yCoordinates);
			transform.scale.SetX(defaultObjScaleX);
			transform.scale.SetY(defaultObjScaleY);
			ecsCoordinator.addComponent(entityObj, transform);
			ecsCoordinator.setEntityID(entityObj, entityId);

			ObjectCreationCondition(items, currentItem, entityObj, entityId);
			DebugSystem::newEntities.push_back(entityObj);


		}


	}
	ImGui::SameLine();

	//For creating entity with random position
	if (ImGui::Button("Random")) {
		for (int i = 0; i < numEntitiesToCreate; i++) {

			entityObj = ecsCoordinator.createEntity();

			entityId = sigBuffer;

			TransformComponent transform{};

			transform.position = myMath::Vector2D(ecsCoordinator.getRandomVal(coordinateMinLimitsX, coordinateMaxLimitsX),
				ecsCoordinator.getRandomVal(coordinateMinLimitsY, coordinateMaxLimitsY));
			transform.scale.SetX(defaultObjScaleX);
			transform.scale.SetY(defaultObjScaleY);
			ecsCoordinator.addComponent(entityObj, transform);
			ecsCoordinator.setEntityID(entityObj, entityId);

			ObjectCreationCondition(items, currentItem, entityObj, entityId);
			DebugSystem::newEntities.push_back(entityObj);


		}
	}

	//Button to destroy all existing entity
	if (ImGui::Button("Remove All Entity")) {
		for (auto entity : ecsCoordinator.getAllLiveEntities()) {
			if (ecsCoordinator.getEntityID(entity) != "placeholderentity") {
				ecsCoordinator.destroyEntity(entity);
			}
		}
	}


	ImGui::NewLine();
	ImGui::End();
}

//Creates appropriate components based on object type
void ObjectCreation::ObjectCreationCondition(const char* items[], int itemIndex, Entity entityObj, std::string entityId) {

	// Get transform for AABB calculations
	auto& transform = ecsCoordinator.getComponent<TransformComponent>(entityObj);

	if (!strcmp(items[itemIndex], "Enemy")) {
		// For a new enemy entity, set default values
		EnemyComponent enemy{};
		enemy.isEnemy = true;  // Set default for new enemy

		MovementComponent movement{};
		movement.speed = 5.0f;  // Default speed

		PhysicsComponent forces{};
		forces.mass = 1.0f;
		forces.gravityScale = { -0.98f, -0.98f };
		forces.jump = 0.8f;
		forces.dampening = 0.9f;
		forces.maxVelocity = 0.0f;
		forces.force.SetDirection({ 0.0f, 0.0f });
		forces.force.SetMagnitude(0.05f);

		// Calculate AABB based on transform
		AABBComponent aabb{};
		float halfWidth = transform.scale.GetX() / 2.0f;
		float halfHeight = transform.scale.GetY() / 2.0f;
		aabb.left = transform.position.GetX() - halfWidth;
		aabb.right = transform.position.GetX() + halfWidth;
		aabb.top = transform.position.GetY() + halfHeight;
		aabb.bottom = transform.position.GetY() - halfHeight;

		ecsCoordinator.addComponent(entityObj, enemy);
		ecsCoordinator.addComponent(entityObj, movement);
		ecsCoordinator.addComponent(entityObj, forces);
		ecsCoordinator.addComponent(entityObj, aabb);
	}
	else if (!strcmp(items[itemIndex], "Player")) {

		AnimationComponent animation{};
		animation.isAnimated = true;

		// Calculate AABB based on transform
		AABBComponent aabb{};
		float halfWidth = transform.scale.GetX() / 2.0f;
		float halfHeight = transform.scale.GetY() / 2.0f;
		aabb.left = transform.position.GetX() - halfWidth;
		aabb.right = transform.position.GetX() + halfWidth;
		aabb.top = transform.position.GetY() + halfHeight;
		aabb.bottom = transform.position.GetY() - halfHeight;

		PhysicsComponent forces{};
		forces.mass = 1.0f;
		forces.gravityScale = { 9.8f, 9.8f };
		forces.jump = 0.0f;
		forces.dampening = 0.9f;
		forces.maxVelocity = 200.0f;
		forces.force.SetDirection({ 0.0f, 0.0f });
		forces.force.SetMagnitude(10.0f);
		forces.maxAccumulatedForce = 40.0f;

		ecsCoordinator.addComponent(entityObj, animation);
		ecsCoordinator.addComponent(entityObj, aabb);
		ecsCoordinator.addComponent(entityObj, forces);
	}
	else if (!strcmp(items[itemIndex], "Platform")) {
		// Calculate AABB based on transform
		AABBComponent aabb{};
		float halfWidth = transform.scale.GetX() / 2.0f;
		float halfHeight = transform.scale.GetY() / 2.0f;
		aabb.left = transform.position.GetX() - halfWidth;
		aabb.right = transform.position.GetX() + halfWidth;
		aabb.top = transform.position.GetY() + halfHeight;
		aabb.bottom = transform.position.GetY() - halfHeight;

		ClosestPlatform closestPlatform{};
		closestPlatform.isClosest = (transform.orientation.GetX() == 0); // Set true only for flat platform

		ecsCoordinator.addComponent(entityObj, aabb);
		ecsCoordinator.addComponent(entityObj, closestPlatform);
	}
	else if (!strcmp(items[itemIndex], "TextBox")) {
		// Calculate AABB based on transform
		FontComponent fontComp{};
		fontComp.text = textBuffer;
		fontComp.textScale = textScale;
		fontComp.fontId = "Journey";
		fontComp.textBoxWidth = textBoxMinLimit;
		fontComp.color.SetX(1);
		fontComp.color.SetY(2);
		fontComp.color.SetZ(3);

		ecsCoordinator.addComponent(entityObj, fontComp);
	}
	else if (!strcmp(items[itemIndex], "Background")) {

	}
}

void ObjectCreation::LoadObjectCreationFromJSON(std::string const& filename)
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

	serializer.ReadFloat(textScaleMinLimit, "Creation.textScaleMinLimit");

	serializer.ReadFloat(defaultObjScaleX, "Creation.defaultObjScaleX");
	serializer.ReadFloat(defaultObjScaleY, "Creation.defaultObjScaleY");

	serializer.ReadFloat(coordinateMaxLimitsX, "Creation.coordinateMaxLimitsX");
	serializer.ReadFloat(coordinateMaxLimitsY, "Creation.coordinateMaxLimitsY");
	serializer.ReadFloat(coordinateMinLimitsX, "Creation.coordinateMinLimitsX");
	serializer.ReadFloat(coordinateMinLimitsY, "Creation.coordinateMinLimitsY");
	serializer.ReadFloat(orientationMaxLimit, "Creation.orientationMaxLimit");
	serializer.ReadFloat(orientationMinLimit, "Creation.orientationMinLimit");

	serializer.ReadInt(currentItem, "Creation.currentItem");
	serializer.ReadInt(minEntitesToCreate, "Creation.minEntitesToCreate");
	serializer.ReadInt(numEntitiesToCreate, "Creation.numEntitiesToCreate");
	serializer.ReadCharArray(numBuffer, MAXBUFFERSIZE, "Creation.numBuffer");
	serializer.ReadCharArray(textBuffer, MAXTEXTSIZE, "Creation.textBuffer");
	serializer.ReadCharArray(textScaleBuffer, MAXBUFFERSIZE, "Creation.textScaleBuffer");
	serializer.ReadCharArray(xCoordinatesBuffer, MAXBUFFERSIZE, "Creation.xCoordinatesBuffer");
	serializer.ReadCharArray(yCoordinatesBuffer, MAXBUFFERSIZE, "Creation.yCoordinatesBuffer");
	serializer.ReadCharArray(xOrientationBuffer, MAXBUFFERSIZE, "Creation.xOrientationBuffer");
	serializer.ReadCharArray(yOrientationBuffer, MAXBUFFERSIZE, "Creation.yOrientationBuffer");
	serializer.ReadFloat(xOrientation, "Creation.xOrientation");
	serializer.ReadFloat(yOrientation, "Creation.yOrientation");
	serializer.ReadFloat(textScale, "Creation.textScale");

	serializer.ReadFloat(objAttributeSliderMaxLength, "Creation.objAttributeSliderMaxLength");
	serializer.ReadFloat(objAttributeSliderMidLength, "Creation.objAttributeSliderMidLength");
	serializer.ReadFloat(objAttributeSliderMinLength, "Creation.objAttributeSliderMinLength");

	serializer.ReadInt(objCount, "Creation.objCount");

	serializer.ReadFloat(textBoxMaxLimit, "Creation.textBoxMaxLimit");
	serializer.ReadFloat(textBoxMinLimit, "Creation.textBoxMinLimit");
}