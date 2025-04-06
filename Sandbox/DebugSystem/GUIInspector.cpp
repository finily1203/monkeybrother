/*
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Lew Zong Han Owen (z.lew)
@team   :  MonkeHood
@course :  CSD2401
@file   :  GUIInspector.cpp
@brief  :  This file contains the function definition of ImGui Inspector system

*Lew Zong Han Owen (z.lew) :
		- Integrated ImGui inspector window to capture selected entity's transformation data
		- Integrated entity to mouse interaction to drag, scale, rotate, and delete
		- Integrated the feature to allow re-assigning of pre-existing entity behavior

File Contributions: Lew Zong Han Owen (100%)

/*_______________________________________________________________________________________________________________*/
#include "GUIInspector.h"
#include "GUIGameViewport.h"
#include "Debug.h"
#include "FontSystemECS.h"
#include "LogicSystemECS.h"
#include "BehaviourComponent.h"
#include "UIComponent.h"
#include "PlayerBehaviour.h"
#include "EnemyBehaviour.h"
#include "CollectableBehaviour.h"
#include "EffectPumpBehaviour.h"
#include "ExitBehaviour.h"
#include "PlatformBehaviour.h"
#include "FilterBehaviour.h"
#include "MovPlatformBehaviour.h"
#include "GUIAssetBrowser.h"
#include "BackgroundComponent.h"
#include <memory>

std::vector<std::pair<int, std::string>>* Inspector::overlappingEntities;
float Inspector::objAttributeSliderMaxLength;
char Inspector::textBuffer[MAXTEXTSIZE];
ImVec2 Inspector::mouseWorldPos;
glm::mat4 projectionMatrix(1.0f);
int currentItem = 0;
std::vector<std::string>* Inspector::assetNames;
int Inspector::selectEntityID = -1;
int Inspector::draggedEntityID = -1;
bool Inspector::isSelectingEntity = false;
bool moveable = false;
bool checked = false;
bool isOver = false;
bool checks[4] = { false };
static int selectedLayer = 0;

static float thumbnailSize = 256.0f;
static float paddingSize = 20.0f;

float frameTime = 0.01f;  // Starting at 0.01
int rows = 1;
int columns = 1;
int frames = 1;
bool playerExists = false;

bool Inspector::isCtrlDragging = false;
int Inspector::duplicatedEntity = -1;
bool Inspector::hasDuplicated = false;

bool Inspector::isDraggingCopies = false;
std::set<std::pair<int, int>>* Inspector::filledCells;

void Inspector::Initialise() {
	overlappingEntities = new std::vector<std::pair<int, std::string>>();
	LoadInspectorFromJSON(FilePathManager::GetIMGUIInspectorJSONPath());
	if (!assetNames)
	{
		assetNames = new std::vector<std::string>();
	}
	assetNames->clear();

	for (auto& asset : assetsManager.getAssetList())
	{
		assetNames->push_back(asset);
		std::cout << asset << std::endl;
	}

	isDraggingCopies = false;
	filledCells = new std::set<std::pair<int, int>>();
}

void Inspector::Update() {

	mouseWorldPos = GameViewWindow::GetMouseWorldPosition();

	ImVec2 viewportPos = GameViewWindow::getViewportPos(); // Get viewport's position
	ImVec2 mouseScreenPos = ImGui::GetMousePos();
	mouseScreenPos.x -= viewportPos.x;  // Adjust for viewport offset
	mouseScreenPos.y -= viewportPos.y;

	mouseScreenPos.x = (2.0f * mouseScreenPos.x / GLFWFunctions::windowWidth) - 1.0f;
	mouseScreenPos.y = 1.0f - (2.0f * mouseScreenPos.y / GLFWFunctions::windowHeight);

	static int chosenEntityID = -1;   // Entity selected for potential deletion
	bool openDeletePopup = false;

	Console::GetLog() << mouseWorldPos.x << "," << mouseWorldPos.y << std::endl;

	projectionMatrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
	ImVec2 centeredMouse = GameViewWindow::GetCenteredMousePosition();
	Console::GetLog() << "Mouse Relative: X: " << centeredMouse.x << " Y: " << centeredMouse.y << std::endl;
	auto isMouseOverEntity = [&](int entity, float& distanceSquared, bool useCircular = false) -> bool {
		if (ecsCoordinator.hasComponent<ButtonComponent>(entity) || ecsCoordinator.hasComponent<UIComponent>(entity)) {
			const auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);

			float left = -transform.scale.GetX() / 2.0f;
			float right = transform.scale.GetX() / 2.0f;
			float top = transform.scale.GetY() / 2.0f;
			float bottom = -transform.scale.GetY() / 2.0f;

			// Transform mouse position to local space
			myMath::Vector2D localMouse(
				centeredMouse.x - transform.position.GetX(),
				centeredMouse.y - transform.position.GetY()
			);

			// Rotate mouse position back
			float angle = -transform.orientation.GetX() * (3.14159f / 180.0f); // Convert to radians
			float cosAngle = cos(angle);
			float sinAngle = sin(angle);
			myMath::Vector2D rotatedMouse(
				localMouse.GetX() * cosAngle - localMouse.GetY() * sinAngle,
				localMouse.GetX() * sinAngle + localMouse.GetY() * cosAngle
			);

			// Apply collision scale
			float collisionScale = 1.8f;
			float scaledLeft = left * collisionScale;
			float scaledRight = right * collisionScale;
			float scaledTop = top * collisionScale;
			float scaledBottom = bottom * collisionScale;

			return (rotatedMouse.GetX() >= scaledLeft && rotatedMouse.GetX() <= scaledRight &&
				rotatedMouse.GetY() >= scaledBottom && rotatedMouse.GetY() <= scaledTop);
		}
		else
			if (ecsCoordinator.hasComponent<FontComponent>(entity) &&
				ecsCoordinator.hasComponent<TransformComponent>(entity)) {
				const auto& transform = ecsCoordinator.getComponent<TransformComponent>(entity);

				float x = transform.position.GetX();
				float y = transform.position.GetY();
				float width = 400.0f;
				float height = 100.0f;  // Increased from 50 to 100

				// Center the collision box horizontally relative to the position
				float leftBound = x;
				float rightBound = x + (width * 0.4f);
				float topBound = y + height * 0.8f;    // Increased upper bound
				float bottomBound = y - height * 0.2f;  // Lowered bottom bound

				// Calculate distance for circular collision from centered position
				float centerX = x;  // Use position directly since bounds are centered
				float centerY = bottomBound + height * 0.1f;
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

	//std::string selEntityID; // Store selected entity ID instead of Entity handle


	static bool isInitialClickAfterSelection = true;

	static ImVec2 mousePos;
	mousePos = ImGui::GetMousePos();

	static bool initiatedByDoubleClick = false;

	if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && GameViewWindow::IsPointInViewport(mousePos.x, mousePos.y)) {
		selectEntityID = -1;
		isSelectingEntity = true;
		isInitialClickAfterSelection = true;
		overlappingEntities->clear();

		// Get all entities and check for collision
		for (auto entity : ecsCoordinator.getAllLiveEntities()) {
			if (ecsCoordinator.getEntityID(entity) != "placeholderentity") {
				float distSq;
				if (isMouseOverEntity(entity, distSq)) {
					overlappingEntities->push_back({ entity, ecsCoordinator.getEntityID(entity) });
				}
			}
		}

		// Sort overlapping entities by distance if needed
		if (overlappingEntities->size() > 1) {
			std::sort(overlappingEntities->begin(), overlappingEntities->end(),
				[](const auto& a, const auto& b) {
					return a.second < b.second;
				});
		}

		if (!overlappingEntities->empty()) {
			ImGui::OpenPopup("Select Entity");
		}
		else {
			selectEntityID = -1;
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

		for (const auto& [entity, name] : *overlappingEntities) {
			if (ImGui::MenuItem(name.c_str())) {
				if (!GameViewWindow::getPaused()) {
					GameViewWindow::TogglePause();
				}
				selectEntityID = entity;
				draggedEntityID = entity;
				isSelectingEntity = false;
				isInitialClickAfterSelection = true;
				//initiatedByDoubleClick = false;  // Reset double-click state on new selection
				ImGui::CloseCurrentPopup();
				break;
			}
		}
		ImGui::EndPopup();
	}

	// Handle entity dragging
	if (draggedEntityID != -1 && !isSelectingEntity) {
		auto& transform = ecsCoordinator.getComponent<TransformComponent>(draggedEntityID);
		if (ecsCoordinator.hasComponent<FontComponent>(draggedEntityID) || ecsCoordinator.hasComponent<ButtonComponent>(draggedEntityID)
			|| ecsCoordinator.hasComponent<UIComponent>(draggedEntityID)) {
			float left = -transform.scale.GetX() / 2.0f;
			float right = transform.scale.GetX() / 2.0f;
			float top = transform.scale.GetY() / 2.0f;
			float bottom = -transform.scale.GetY() / 2.0f;

			myMath::Vector2D localMouse(
				centeredMouse.x - transform.position.GetX(),
				centeredMouse.y - transform.position.GetY()
			);

			float angle = -transform.orientation.GetX() * (3.14159f / 180.0f);
			float cosAngle = cos(angle);
			float sinAngle = sin(angle);
			myMath::Vector2D rotatedMouse(
				localMouse.GetX()* cosAngle - localMouse.GetY() * sinAngle,
				localMouse.GetX()* sinAngle + localMouse.GetY() * cosAngle
			);

			float collisionScale = 1.8f;
			float scaledLeft = left * collisionScale;
			float scaledRight = right * collisionScale;
			float scaledTop = top * collisionScale;
			float scaledBottom = bottom * collisionScale;

			isOver = (rotatedMouse.GetX() >= scaledLeft && rotatedMouse.GetX() <= scaledRight &&
				rotatedMouse.GetY() >= scaledBottom && rotatedMouse.GetY() <= scaledTop);
		}
		else {
			float dx = mouseWorldPos.x - transform.position.GetX();
			float dy = mouseWorldPos.y - transform.position.GetY();
			float halfWidth = std::abs(transform.scale.GetX() / 2.0f);
			float halfHeight = std::abs(transform.scale.GetY() / 2.0f);
			bool withinX = std::abs(dx) <= halfWidth;
			bool withinY = std::abs(dy) <= halfHeight;
			if (withinX && withinY)
				isOver = true;
			else
				isOver = false;
		}

		// Handle entity dragging
		if (ImGui::IsMouseDown(ImGuiMouseButton_Left) &&
			GameViewWindow::IsPointInViewport(mousePos.x, mousePos.y)) {

			bool isCtrlPressed = ImGui::GetIO().KeyCtrl;

			if (isOver) {
				moveable = true;

				// Start Ctrl+drag copy operation
				if (isCtrlPressed && !isDraggingCopies && draggedEntityID != -1) {
					isDraggingCopies = true;
					duplicatedEntity = draggedEntityID;
					filledCells->clear();

					// Add the original entity's cell to filled cells
					auto& transformation = ecsCoordinator.getComponent<TransformComponent>(duplicatedEntity);
					myMath::Vector2D origPos = transformation.position;
					myMath::Vector2D snappedOrigPos = gridSystem.snapToGrid(origPos);
					int origCellX = static_cast<int>(snappedOrigPos.GetX() / gridSystem.getCellSize());
					int origCellY = static_cast<int>(snappedOrigPos.GetY() / gridSystem.getCellSize());
					filledCells->insert({ origCellX, origCellY });
				}
			}

			if (moveable) {
				// Regular dragging behavior for non-Ctrl case
				if (!isCtrlPressed) {
					if (isDraggingCopies) {
						// User released Ctrl while dragging - stop copy mode
						isDraggingCopies = false;
						filledCells->clear();
					}

					// Normal entity dragging (unchanged)
					if (ecsCoordinator.hasComponent<FontComponent>(draggedEntityID) ||
						ecsCoordinator.hasComponent<ButtonComponent>(draggedEntityID) ||
						ecsCoordinator.hasComponent<UIComponent>(draggedEntityID)) {
						transform.position.SetX(centeredMouse.x);
						transform.position.SetY(centeredMouse.y);
					}
					else {
						transform.position.SetX(mouseWorldPos.x);
						transform.position.SetY(mouseWorldPos.y);
					}
				}
				// Ctrl+drag copy mode
				else if (isDraggingCopies) {
					// Convert mouse position to grid cell
					myMath::Vector2D mousePosition(mouseWorldPos.x, mouseWorldPos.y);
					myMath::Vector2D snappedMousePos = gridSystem.snapToGrid(mousePosition);
					int mouseCellX = static_cast<int>(snappedMousePos.GetX() / gridSystem.getCellSize());
					int mouseCellY = static_cast<int>(snappedMousePos.GetY() / gridSystem.getCellSize());

					// Check if this is a new cell that we haven't filled yet
					if (filledCells->find({ mouseCellX, mouseCellY }) == filledCells->end()) {
						// Create new entity at this cell
						Entity newEntity = CloneEntity(duplicatedEntity);

						// Position at the cell's center
						auto& newTransform = ecsCoordinator.getComponent<TransformComponent>(newEntity);
						newTransform.position = snappedMousePos;

						// Mark this cell as filled
						filledCells->insert({ mouseCellX, mouseCellY });
					}
				}
			}
		}
		else { // Mouse released
			if (isDraggingCopies) {
				isDraggingCopies = false;
				filledCells->clear();
			}

			if (moveable) {
				// Snap to grid if in tile map mode
				if (GameViewWindow::isTileMapMode()) {
					myMath::Vector2D currentPos = transform.position;
					myMath::Vector2D snappedPos = gridSystem.snapToGrid(currentPos);
					transform.position = snappedPos;
				}

				moveable = false;
			}
		}
	}

	// Handle mouse wheel scaling for selected entity
	if (selectEntityID != -1 && ImGui::GetIO().MouseWheel != 0.0f && GameViewWindow::IsPointInViewport(ImGui::GetMousePos().x, ImGui::GetMousePos().y)) {
		float wheel_delta = ImGui::GetIO().MouseWheel;
		auto& transform = ecsCoordinator.getComponent<TransformComponent>(selectEntityID);
		float dx = mouseWorldPos.x - transform.position.GetX();
		float dy = mouseWorldPos.y - transform.position.GetY();
		float halfWidth = std::abs(transform.scale.GetX() / 2.0f);
		float halfHeight = std::abs(transform.scale.GetY() / 2.0f);

		// Check if mouse is within bounds, accounting for any combination of negative/positive scales
		bool withinX = std::abs(dx) <= halfWidth;
		bool withinY = std::abs(dy) <= halfHeight;
		float scaleFactor = 1.0f + (wheel_delta * 0.1f);

		if (withinX && withinY) {
			if (!ecsCoordinator.hasComponent<PlayerComponent>(selectEntityID)) {
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
	if (ImGui::IsKeyPressed(ImGuiKey_Delete) && selectEntityID != -1) {
		float distSq;
		if (isMouseOverEntity(selectEntityID, distSq)) {
			openDeletePopup = true;
		}
	}

	// Handle selection mode cancellation
	if (isSelectingEntity && !ImGui::IsPopupOpen("Select Entity")) {
		isSelectingEntity = false;
	}

	RenderInspectorWindow(ecsCoordinator, selectEntityID);

}

// Render the Inspector window + logic for modifying entity data
void Inspector::RenderInspectorWindow(ECSCoordinator& ecs, int selectedEntityID) {
    auto logicSystemRef = ecsCoordinator.getSpecificSystem<LogicSystemECS>();
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

    // Add a Components button that opens a popup with all available components
    if (ImGui::Button("Edit Components")) {
        ImGui::OpenPopup("Component Editor");
    }

    // Component Editor Popup
    if (ImGui::BeginPopupModal("Component Editor", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Add or remove components for entity: %s", entityID.c_str());
        ImGui::Separator();

        // Create checkboxes for each component type
        // TransformComponent is a special case - all entities must have it
        bool hasTransform = ecs.hasComponent<TransformComponent>(selectedEntityID);
        ImGui::BeginDisabled();  // Disable the checkbox because all entities must have transforms
        ImGui::Checkbox("Transform Component", &hasTransform);
        ImGui::EndDisabled();
        ImGui::SameLine();
        ImGui::TextDisabled("(Required)");

        // AABB Component
        bool hasAABB = ecs.hasComponent<AABBComponent>(selectedEntityID);
        if (ImGui::Checkbox("AABB Component", &hasAABB)) {
            if (hasAABB && !ecs.hasComponent<AABBComponent>(selectedEntityID)) {
                AABBComponent aabb;
                // Initialize with default values based on entity's transform
                auto& transform = ecs.getComponent<TransformComponent>(selectedEntityID);
                aabb.left = -transform.scale.GetX() / 2.0f;
                aabb.right = transform.scale.GetX() / 2.0f;
                aabb.top = transform.scale.GetY() / 2.0f;
                aabb.bottom = -transform.scale.GetY() / 2.0f;
                ecs.addComponent(selectedEntityID, aabb);
            }
            else if (!hasAABB && ecs.hasComponent<AABBComponent>(selectedEntityID)) {
                ecs.removeComponent<AABBComponent>(selectedEntityID);
            }
        }

        // Physics Component
        bool hasPhysics = ecs.hasComponent<PhysicsComponent>(selectedEntityID);
        if (ImGui::Checkbox("Physics Component", &hasPhysics)) {
            if (hasPhysics && !ecs.hasComponent<PhysicsComponent>(selectedEntityID)) {
                PhysicsComponent physics;
                // Initialize with reasonable defaults
                physics.mass = 1.0f;
                physics.gravityScale = myMath::Vector2D(9.8f, 9.8f);
                physics.dampening = 0.9f;
                physics.maxVelocity = 200.0f;
                ecs.addComponent<PhysicsComponent>(selectedEntityID, physics);
            }
            else if (!hasPhysics && ecs.hasComponent<PhysicsComponent>(selectedEntityID)) {
                ecs.removeComponent<PhysicsComponent>(selectedEntityID);
            }
        }

        // Animation Component
        bool hasAnimation = ecs.hasComponent<AnimationComponent>(selectedEntityID);
        if (ImGui::Checkbox("Animation Component", &hasAnimation)) {
            if (hasAnimation && !ecs.hasComponent<AnimationComponent>(selectedEntityID)) {
                AnimationComponent animation;
                animation.isAnimated = true;
                animation.totalFrames = 1.0f;
                animation.frameTime = 0.05f;
                animation.columns = 1.0f;
                animation.rows = 1.0f;
                ecs.addComponent<AnimationComponent>(selectedEntityID, animation);
            }
            else if (!hasAnimation && ecs.hasComponent<AnimationComponent>(selectedEntityID)) {
                ecs.removeComponent<AnimationComponent>(selectedEntityID);
            }
        }

        // Player Component
        bool hasPlayer = ecs.hasComponent<PlayerComponent>(selectedEntityID);
        if (ImGui::Checkbox("Player Component", &hasPlayer)) {
            // Check if a player already exists
            bool playerExist = false;
            if (hasPlayer && !ecs.hasComponent<PlayerComponent>(selectedEntityID)) {
                for (auto& entity : ecs.getAllLiveEntities()) {
                    if ((int)entity != selectedEntityID && ecs.hasComponent<PlayerComponent>(entity)) {
                        playerExist = true;
                        break;
                    }
                }

                if (!playerExist) {
                    PlayerComponent player;
                    player.isPlayer = true;
                    ecs.addComponent<PlayerComponent>(selectedEntityID, player);
                }
                else {
                    // Only one player allowed - show a warning
                    ImGui::OpenPopup("Player Warning");
                    hasPlayer = false;
                }
            }
            else if (!hasPlayer && ecs.hasComponent<PlayerComponent>(selectedEntityID)) {
                ecs.removeComponent<PlayerComponent>(selectedEntityID);
            }
        }

        // Warning popup for player component
        if (ImGui::BeginPopupModal("Player Warning", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Only one player entity can exist in the scene.");
            ImGui::Separator();
            if (ImGui::Button("OK", ImVec2(120, 0))) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        // Enemy Component
        bool hasEnemy = ecs.hasComponent<EnemyComponent>(selectedEntityID);
        if (ImGui::Checkbox("Enemy Component", &hasEnemy)) {
            if (hasEnemy && !ecs.hasComponent<EnemyComponent>(selectedEntityID)) {
                EnemyComponent enemy;
                enemy.isEnemy = true;
                enemy.isClockwise = true;
                enemy.visionAngle = 60.0f;
                enemy.visionDistance = 300.0f;
                enemy.drawVisionDebug = true;
                ecs.addComponent<EnemyComponent>(selectedEntityID, enemy);
            }
            else if (!hasEnemy && ecs.hasComponent<EnemyComponent>(selectedEntityID)) {
                ecs.removeComponent<EnemyComponent>(selectedEntityID);
            }
        }

        // Collectable Component
        bool hasCollectable = ecs.hasComponent<CollectableComponent>(selectedEntityID);
        if (ImGui::Checkbox("Collectable Component", &hasCollectable)) {
            if (hasCollectable && !ecs.hasComponent<CollectableComponent>(selectedEntityID)) {
                CollectableComponent collectable;
                collectable.isCollectable = true;
                ecs.addComponent<CollectableComponent>(selectedEntityID, collectable);
                GLFWFunctions::collectableCount++;
            }
            else if (!hasCollectable && ecs.hasComponent<CollectableComponent>(selectedEntityID)) {
                ecs.removeComponent<CollectableComponent>(selectedEntityID);
                if (GLFWFunctions::collectableCount > 0)
                    GLFWFunctions::collectableCount--;
            }
        }

        // Pump Component
        bool hasPump = ecs.hasComponent<PumpComponent>(selectedEntityID);
        if (ImGui::Checkbox("Pump Component", &hasPump)) {
            if (hasPump && !ecs.hasComponent<PumpComponent>(selectedEntityID)) {
                PumpComponent pump;
                pump.isPump = true;
                pump.pumpForce = 3.0f;
                ecs.addComponent<PumpComponent>(selectedEntityID, pump);
            }
            else if (!hasPump && ecs.hasComponent<PumpComponent>(selectedEntityID)) {
                ecs.removeComponent<PumpComponent>(selectedEntityID);
            }
        }

        // Exit Component
        bool hasExit = ecs.hasComponent<ExitComponent>(selectedEntityID);
        if (ImGui::Checkbox("Exit Component", &hasExit)) {
            if (hasExit && !ecs.hasComponent<ExitComponent>(selectedEntityID)) {
                ExitComponent exit;
                exit.isExit = true;
                ecs.addComponent<ExitComponent>(selectedEntityID, exit);
            }
            else if (!hasExit && ecs.hasComponent<ExitComponent>(selectedEntityID)) {
                ecs.removeComponent<ExitComponent>(selectedEntityID);
            }
        }

        // Background Component
        bool hasBackground = ecs.hasComponent<BackgroundComponent>(selectedEntityID);
        if (ImGui::Checkbox("Background Component", &hasBackground)) {
            if (hasBackground && !ecs.hasComponent<BackgroundComponent>(selectedEntityID)) {
                BackgroundComponent background;
                background.isBackground = true;
                ecs.addComponent<BackgroundComponent>(selectedEntityID, background);
            }
            else if (!hasBackground && ecs.hasComponent<BackgroundComponent>(selectedEntityID)) {
                ecs.removeComponent<BackgroundComponent>(selectedEntityID);
            }
        }

        // UI Component
        bool hasUI = ecs.hasComponent<UIComponent>(selectedEntityID);
        if (ImGui::Checkbox("UI Component", &hasUI)) {
            if (hasUI && !ecs.hasComponent<UIComponent>(selectedEntityID)) {
                UIComponent ui;
                ui.isUI = true;
                ecs.addComponent<UIComponent>(selectedEntityID, ui);
            }
            else if (!hasUI && ecs.hasComponent<UIComponent>(selectedEntityID)) {
                ecs.removeComponent<UIComponent>(selectedEntityID);
            }
        }

        // Button Component
        bool hasButton = ecs.hasComponent<ButtonComponent>(selectedEntityID);
        if (ImGui::Checkbox("Button Component", &hasButton)) {
            if (hasButton && !ecs.hasComponent<ButtonComponent>(selectedEntityID)) {
                ButtonComponent button;
                auto& transform = ecs.getComponent<TransformComponent>(selectedEntityID);
                button.originalScale = transform.scale;
                button.hoveredScale = myMath::Vector2D(transform.scale.GetX() * 1.1f, transform.scale.GetY() * 1.1f);
                button.isButton = true;
                ecs.addComponent<ButtonComponent>(selectedEntityID, button);
            }
            else if (!hasButton && ecs.hasComponent<ButtonComponent>(selectedEntityID)) {
                ecs.removeComponent<ButtonComponent>(selectedEntityID);
            }
        }

        // Filter Component
        bool hasFilter = ecs.hasComponent<FilterComponent>(selectedEntityID);
        if (ImGui::Checkbox("Filter Component", &hasFilter)) {
            if (hasFilter && !ecs.hasComponent<FilterComponent>(selectedEntityID)) {
                FilterComponent filter;
                filter.isFilter = true;
                filter.isFilterClogged = false;
                ecs.addComponent<FilterComponent>(selectedEntityID, filter);
            }
            else if (!hasFilter && ecs.hasComponent<FilterComponent>(selectedEntityID)) {
                ecs.removeComponent<FilterComponent>(selectedEntityID);
            }
        }

        // Moving Platform Component
        bool hasMovPlatform = ecs.hasComponent<MovPlatformComponent>(selectedEntityID);
        if (ImGui::Checkbox("Moving Platform Component", &hasMovPlatform)) {
            if (hasMovPlatform && !ecs.hasComponent<MovPlatformComponent>(selectedEntityID)) {
                MovPlatformComponent movPlatform;
                auto& transform = ecs.getComponent<TransformComponent>(selectedEntityID);
                movPlatform.speed = 50.0f;
                movPlatform.maxDistance = 200.0f;
                movPlatform.startPos = transform.position;
                movPlatform.direction = myMath::Vector2D(1.0f, 0.0f);  // Default horizontal movement
                ecs.addComponent<MovPlatformComponent>(selectedEntityID, movPlatform);
            }
            else if (!hasMovPlatform && ecs.hasComponent<MovPlatformComponent>(selectedEntityID)) {
                ecs.removeComponent<MovPlatformComponent>(selectedEntityID);
            }
        }

        // Navigation Component
        bool hasNavigation = ecs.hasComponent<NavigationComponent>(selectedEntityID);
        if (ImGui::Checkbox("Navigation Component", &hasNavigation)) {
            if (hasNavigation && !ecs.hasComponent<NavigationComponent>(selectedEntityID)) {
                NavigationComponent navigation;
                navigation.isNavigation = true;
                navigation.isVisible = true;
                ecs.addComponent<NavigationComponent>(selectedEntityID, navigation);
            }
            else if (!hasNavigation && ecs.hasComponent<NavigationComponent>(selectedEntityID)) {
                ecs.removeComponent<NavigationComponent>(selectedEntityID);
            }
        }

        // Platform Component
        bool hasClosestPlatform = ecs.hasComponent<ClosestPlatform>(selectedEntityID);
        if (ImGui::Checkbox("Platform Component", &hasClosestPlatform)) {
            if (hasClosestPlatform && !ecs.hasComponent<ClosestPlatform>(selectedEntityID)) {
                ClosestPlatform closestPlatform{};
                closestPlatform.isClosest = false;
                ecs.addComponent<ClosestPlatform>(selectedEntityID, closestPlatform);

                // Automatically assign platform behavior when adding platform component
                logicSystemRef->assignBehaviour(selectedEntityID, std::make_shared<PlatformBehaviour>());

                // If you also need to update the behavior dropdown selection
                currentItem = 6;
            }
            else if (!hasClosestPlatform && ecs.hasComponent<ClosestPlatform>(selectedEntityID)) {
                ecs.removeComponent<ClosestPlatform>(selectedEntityID);
            }
        }

        ImGui::Separator();
        if (ImGui::Button("Close", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    // Always display Transform Component properties
    if (ecsCoordinator.hasComponent<TransformComponent>(selectedEntityID)) {
        if (ImGui::CollapsingHeader("Transform Component", ImGuiTreeNodeFlags_DefaultOpen)) {
            auto& transform = ecsCoordinator.getComponent<TransformComponent>(selectedEntityID);

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
        }
    }

    // AABB Component
    if (ecsCoordinator.hasComponent<AABBComponent>(selectedEntityID)) {
        if (ImGui::CollapsingHeader("AABB Component")) {
            auto& aabb = ecsCoordinator.getComponent<AABBComponent>(selectedEntityID);

            ImGui::DragFloat("Left", &aabb.left, 1.0f);
            ImGui::DragFloat("Right", &aabb.right, 1.0f);
            ImGui::DragFloat("Top", &aabb.top, 1.0f);
            ImGui::DragFloat("Bottom", &aabb.bottom, 1.0f);

            // Add button to auto-calculate AABB from transform
            if (ImGui::Button("Calculate from Transform")) {
                auto& transform = ecsCoordinator.getComponent<TransformComponent>(selectedEntityID);
                aabb.left = -transform.scale.GetX() / 2.0f;
                aabb.right = transform.scale.GetX() / 2.0f;
                aabb.top = transform.scale.GetY() / 2.0f;
                aabb.bottom = -transform.scale.GetY() / 2.0f;
            }
        }
    }

    // Physics Component
    if (ecsCoordinator.hasComponent<PhysicsComponent>(selectedEntityID)) {
        if (ImGui::CollapsingHeader("Physics Component")) {
            auto& physics = ecsCoordinator.getComponent<PhysicsComponent>(selectedEntityID);

            ImGui::DragFloat("Mass", &physics.mass, 0.1f, 0.1f, 100.0f);

            float gravity[2] = { physics.gravityScale.GetX(), physics.gravityScale.GetY() };
            if (ImGui::DragFloat2("Gravity Scale", gravity, 0.1f)) {
                physics.gravityScale.SetX(gravity[0]);
                physics.gravityScale.SetY(gravity[1]);
            }

            ImGui::DragFloat("Jump Force", &physics.jump, 1.0f, 0.0f, 1000.0f);
            ImGui::DragFloat("Dampening", &physics.dampening, 0.01f, 0.0f, 1.0f);

            float velocity[2] = { physics.velocity.GetX(), physics.velocity.GetY() };
            if (ImGui::DragFloat2("Velocity", velocity, 1.0f)) {
                physics.velocity.SetX(velocity[0]);
                physics.velocity.SetY(velocity[1]);
            }

            ImGui::DragFloat("Max Velocity", &physics.maxVelocity, 1.0f, 0.0f, 1000.0f);

            float accel[2] = { physics.acceleration.GetX(), physics.acceleration.GetY() };
            if (ImGui::DragFloat2("Acceleration", accel, 0.1f)) {
                physics.acceleration.SetX(accel[0]);
                physics.acceleration.SetY(accel[1]);
            }

            // Force properties
            ImGui::Text("Force:");
            float forceMag = physics.force.GetMagnitude();
            if (ImGui::DragFloat("Magnitude", &forceMag, 0.5f, 0.0f, 100.0f)) {
                physics.force.SetMagnitude(forceMag);
            }

            float forceDir[2] = { physics.force.GetDirection().GetX(), physics.force.GetDirection().GetY() };
            if (ImGui::DragFloat2("Direction", forceDir, 0.01f, -1.0f, 1.0f)) {
                // Normalize direction vector if needed
                float length = sqrt(forceDir[0] * forceDir[0] + forceDir[1] * forceDir[1]);
                if (length > 0.001f) {
                    forceDir[0] /= length;
                    forceDir[1] /= length;
                }
                physics.force.SetDirection(myMath::Vector2D(forceDir[0], forceDir[1]));
            }

            float accForce[2] = { physics.accumulatedForce.GetX(), physics.accumulatedForce.GetY() };
            if (ImGui::DragFloat2("Accumulated Force", accForce, 0.5f)) {
                physics.accumulatedForce.SetX(accForce[0]);
                physics.accumulatedForce.SetY(accForce[1]);
            }

            ImGui::DragFloat("Max Accumulated Force", &physics.maxAccumulatedForce, 1.0f, 0.0f, 1000.0f);
            ImGui::DragFloat("Previous Force", &physics.prevForce, 0.5f);
            ImGui::DragFloat("Target Force", &physics.targetForce, 0.5f);
        }
    }

    // Animation Component
    if (ecsCoordinator.hasComponent<AnimationComponent>(selectedEntityID)) {
        if (ImGui::CollapsingHeader("Animation Component")) {
            auto& animation = ecsCoordinator.getComponent<AnimationComponent>(selectedEntityID);

            // Convert float values to integers for UI input
            frameTime = animation.frameTime;
            rows = static_cast<int>(animation.rows);
            columns = static_cast<int>(animation.columns);
            frames = static_cast<int>(animation.totalFrames);

            // UI Controls
            ImGui::PushItemWidth(100.0f);
            if (ImGui::InputInt("TotalFrames", &frames, 1, 100)) {
                animation.totalFrames = static_cast<float>(frames);
            }

            if (ImGui::InputInt("Rows", &rows, 1, 100)) {
                animation.rows = static_cast<float>(rows);
            }

            if (ImGui::InputInt("Columns", &columns, 1, 100)) {
                animation.columns = static_cast<float>(columns);
            }

            if (ImGui::SliderFloat("Frame Time", &frameTime, 0.01f, 0.10f, "%.2f sec", ImGuiSliderFlags_NoInput)) {
                animation.frameTime = frameTime;
            }
            ImGui::PopItemWidth();

            Console().GetLog() << "rows: " << animation.rows << " columns: " << animation.columns << std::endl;

            GLuint textureID = assetsManager.GetTexture(ecsCoordinator.getTextureID(selectedEntityID));

            ImGui::Image((void*)(intptr_t)textureID, { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
        }
    }

    // Button Component
    if (ecsCoordinator.hasComponent<ButtonComponent>(selectedEntityID)) {
        if (ImGui::CollapsingHeader("Button Component")) {
            auto& button = ecsCoordinator.getComponent<ButtonComponent>(selectedEntityID);

            ImGui::Checkbox("Is Button", &button.isButton);
        }
    }

    // Enemy Component
    if (ecsCoordinator.hasComponent<EnemyComponent>(selectedEntityID)) {
        if (ImGui::CollapsingHeader("Enemy Component")) {
            auto& enemy = ecsCoordinator.getComponent<EnemyComponent>(selectedEntityID);

            ImGui::Checkbox("Is Enemy", &enemy.isEnemy);
            ImGui::Checkbox("Is Clockwise", &enemy.isClockwise);
            ImGui::DragFloat("Vision Angle", &enemy.visionAngle, 1.0f, 0.0f, 360.0f);
            ImGui::DragFloat("Vision Distance", &enemy.visionDistance, 5.0f, 0.0f, 1000.0f);
            ImGui::Checkbox("Draw Vision Debug", &enemy.drawVisionDebug);

            // Display and edit waypoints
            static int selectedWaypoint = -1;
            ImGui::Text("Waypoints (%d):", enemy.numWaypoints);

            // Add new waypoint button
            if (ImGui::Button("Add Waypoint")) {
                // Add new waypoint at a reasonable offset from the last one
                myMath::Vector2D newPos;
                if (enemy.waypoints.empty()) {
                    auto& transform = ecsCoordinator.getComponent<TransformComponent>(selectedEntityID);
                    newPos = transform.position;
                }
                else {
                    newPos = enemy.waypoints.back();
                    newPos.SetX(newPos.GetX() + 100.0f);
                }
                enemy.waypoints.push_back(newPos);
                enemy.numWaypoints = static_cast<int>(enemy.waypoints.size());
            }

            // Remove selected waypoint button
            ImGui::SameLine();
            if (ImGui::Button("Remove Waypoint") && selectedWaypoint >= 0 && selectedWaypoint < enemy.waypoints.size()) {
                enemy.waypoints.erase(enemy.waypoints.begin() + selectedWaypoint);
                enemy.numWaypoints = static_cast<int>(enemy.waypoints.size());
                selectedWaypoint = -1;
            }

            // List all waypoints
            ImGui::BeginChild("WaypointsList", ImVec2(0, 150), true);
            for (int i = 0; i < enemy.waypoints.size(); i++) {
                auto& waypoint = enemy.waypoints[i];
                char label[32];
                sprintf_s(label, "Waypoint %d", i + 1);

                bool isSelected = (selectedWaypoint == i);
                if (ImGui::Selectable(label, isSelected)) {
                    selectedWaypoint = i;
                }

                // Display waypoint coordinates
                ImGui::SameLine(ImGui::GetWindowWidth() - 200);
                ImGui::Text("(%.1f, %.1f)", waypoint.GetX(), waypoint.GetY());
            }
            ImGui::EndChild();

            // Edit selected waypoint
            if (selectedWaypoint >= 0 && selectedWaypoint < enemy.waypoints.size()) {
                ImGui::Text("Edit Waypoint %d:", selectedWaypoint + 1);
                float pos[2] = { enemy.waypoints[selectedWaypoint].GetX(), enemy.waypoints[selectedWaypoint].GetY() };
                if (ImGui::DragFloat2("Position", pos, 5.0f)) {
                    enemy.waypoints[selectedWaypoint].SetX(pos[0]);
                    enemy.waypoints[selectedWaypoint].SetY(pos[1]);
                }
            }

            // State dropdown
            const char* states[] = { "Patrol", "Chase", "Attack" };
            ImGui::Combo("Current State", &enemy.currState, states, IM_ARRAYSIZE(states));
        }
    }

    // Player Component
    if (ecsCoordinator.hasComponent<PlayerComponent>(selectedEntityID)) {
        if (ImGui::CollapsingHeader("Player Component")) {
            auto& player = ecsCoordinator.getComponent<PlayerComponent>(selectedEntityID);

            ImGui::Checkbox("Is Player", &player.isPlayer);
            
        }
    }

    // Pump Component
    if (ecsCoordinator.hasComponent<PumpComponent>(selectedEntityID)) {
        if (ImGui::CollapsingHeader("Pump Component")) {
            auto& pump = ecsCoordinator.getComponent<PumpComponent>(selectedEntityID);

            ImGui::Checkbox("Is Pump", &pump.isPump);
            ImGui::Checkbox("Is Animate", &pump.isAnimate);
            ImGui::DragFloat("Pump Force", &pump.pumpForce, 0.1f, 0.0f, 20.0f);
        }
    }

    // Exit Component
    if (ecsCoordinator.hasComponent<ExitComponent>(selectedEntityID)) {
        if (ImGui::CollapsingHeader("Exit Component")) {
            auto& exit = ecsCoordinator.getComponent<ExitComponent>(selectedEntityID);

            ImGui::Checkbox("Is Exit", &exit.isExit);
        }
    }

    // Collectable Component
    if (ecsCoordinator.hasComponent<CollectableComponent>(selectedEntityID)) {
        if (ImGui::CollapsingHeader("Collectable Component")) {
            auto& collectable = ecsCoordinator.getComponent<CollectableComponent>(selectedEntityID);

            ImGui::Checkbox("Is Collectable", &collectable.isCollectable);
        }
    }

    // Background Component
    if (ecsCoordinator.hasComponent<BackgroundComponent>(selectedEntityID)) {
        if (ImGui::CollapsingHeader("Background Component")) {
            auto& background = ecsCoordinator.getComponent<BackgroundComponent>(selectedEntityID);

            ImGui::Checkbox("Is Background", &background.isBackground);
        }
    }

    // UI Component
    if (ecsCoordinator.hasComponent<UIComponent>(selectedEntityID)) {
        if (ImGui::CollapsingHeader("UI Component")) {
            auto& ui = ecsCoordinator.getComponent<UIComponent>(selectedEntityID);

            ImGui::Checkbox("Is UI", &ui.isUI);
        }
    }

    // Filter Component
    if (ecsCoordinator.hasComponent<FilterComponent>(selectedEntityID)) {
        if (ImGui::CollapsingHeader("Filter Component")) {
            auto& filter = ecsCoordinator.getComponent<FilterComponent>(selectedEntityID);

            ImGui::Checkbox("Is Filter", &filter.isFilter);
            ImGui::Checkbox("Is Filter Clogged", &filter.isFilterClogged);
        }
    }

    // Moving Platform Component
    if (ecsCoordinator.hasComponent<MovPlatformComponent>(selectedEntityID)) {
        if (ImGui::CollapsingHeader("Moving Platform Component")) {
            auto& movPlatform = ecsCoordinator.getComponent<MovPlatformComponent>(selectedEntityID);

            ImGui::DragFloat("Speed", &movPlatform.speed, 1.0f, 0.0f, 200.0f);
            ImGui::DragFloat("Max Distance", &movPlatform.maxDistance, 5.0f, 0.0f, 1000.0f);
            ImGui::Checkbox("Moving Forward", &movPlatform.movForward);

            float startPos[2] = { movPlatform.startPos.GetX(), movPlatform.startPos.GetY() };
            if (ImGui::DragFloat2("Start Position", startPos, 5.0f)) {
                movPlatform.startPos.SetX(startPos[0]);
                movPlatform.startPos.SetY(startPos[1]);
            }

            float direction[2] = { movPlatform.direction.GetX(), movPlatform.direction.GetY() };
            if (ImGui::DragFloat2("Direction", direction, 0.1f, -1.0f, 1.0f)) {
                // Normalize direction
                float length = sqrt(direction[0] * direction[0] + direction[1] * direction[1]);
                if (length > 0.001f) {
                    direction[0] /= length;
                    direction[1] /= length;
                }
                movPlatform.direction.SetX(direction[0]);
                movPlatform.direction.SetY(direction[1]);
            }

            if (ImGui::Button("Set Current Position as Start")) {
                auto& transform = ecsCoordinator.getComponent<TransformComponent>(selectedEntityID);
                movPlatform.startPos = transform.position;
            }

            ImGui::SameLine();

            if (ImGui::Button("Horizontal Movement")) {
                movPlatform.direction.SetX(1.0f);
                movPlatform.direction.SetY(0.0f);
            }

            ImGui::SameLine();

            if (ImGui::Button("Vertical Movement")) {
                movPlatform.direction.SetX(0.0f);
                movPlatform.direction.SetY(1.0f);
            }
        }
    }

    // Navigation Component
    if (ecsCoordinator.hasComponent<NavigationComponent>(selectedEntityID)) {
        if (ImGui::CollapsingHeader("Navigation Component")) {
            auto& navigation = ecsCoordinator.getComponent<NavigationComponent>(selectedEntityID);

            ImGui::Checkbox("Is Navigation", &navigation.isNavigation);
            ImGui::Checkbox("Is Visible", &navigation.isVisible);
        }
    }

    // Closest Platform Component
    if (ecsCoordinator.hasComponent<ClosestPlatform>(selectedEntityID)) {
        if (ImGui::CollapsingHeader("Platform Component")) {
            auto& platform = ecsCoordinator.getComponent<ClosestPlatform>(selectedEntityID);

            ImGui::Checkbox("Is Closest", &platform.isClosest);
        }
    }

    // Add a divider before the behavior section
    ImGui::Separator();

    // Behavior Component
    if (ecsCoordinator.hasComponent<BehaviourComponent>(selectedEntityID)) {
        if (ImGui::CollapsingHeader("Behavior", ImGuiTreeNodeFlags_DefaultOpen)) {
            // Behavior dropdown
            const char* items[] = { "None", "Enemy", "Pump", "Exit", "Collectable", "Player", "Platform", "Button", "Filter", "MovPlatform", "Navigation" };

            if (logicSystemRef->hasBehaviour<EnemyBehaviour>(selectedEntityID)) {
                currentItem = 1;
            }
            else if (logicSystemRef->hasBehaviour<EffectPumpBehaviour>(selectedEntityID)) {
                currentItem = 2;
            }
            else if (logicSystemRef->hasBehaviour<ExitBehaviour>(selectedEntityID)) {
                currentItem = 3;
            }
            else if (logicSystemRef->hasBehaviour<CollectableBehaviour>(selectedEntityID)) {
                currentItem = 4;
            }
            else if (logicSystemRef->hasBehaviour<PlayerBehaviour>(selectedEntityID)) {
                currentItem = 5;
            }
            else if (logicSystemRef->hasBehaviour<PlatformBehaviour>(selectedEntityID)) {
                currentItem = 6;
            }
            else if (logicSystemRef->hasBehaviour<MouseBehaviour>(selectedEntityID)) {
                currentItem = 7;
            }
            else if (logicSystemRef->hasBehaviour<FilterBehaviour>(selectedEntityID)) {
                currentItem = 8;
            }
            else if (logicSystemRef->hasBehaviour<MovPlatformBehaviour>(selectedEntityID)) {
                currentItem = 9;
            }
            else {
                currentItem = 0;
            }

            ImGui::SetNextItemWidth(200);
            if (ImGui::BeginCombo("##BehaviorDropdown", items[currentItem])) {
                for (int i = 0; i < IM_ARRAYSIZE(items); i++) {
                    const bool typeSelected = (currentItem == i);
                    if (ImGui::Selectable(items[i], typeSelected)) {
                        // Add necessary components and remove incompatible ones based on behavior type
                        if (i == 0) { // None
                            if (ecsCoordinator.hasComponent<PlayerComponent>(selectedEntityID)) {
                                ecsCoordinator.removeComponent<PlayerComponent>(selectedEntityID);
                            }
                            if (logicSystemRef->hasBehaviour(selectedEntityID)) {
                                logicSystemRef->unassignBehaviour(selectedEntityID);
                            }
                        }
                        else if (i == 1) { // Enemy
                            if (!ecsCoordinator.hasComponent<PhysicsComponent>(selectedEntityID)) {
                                PhysicsComponent physics;
                                physics.gravityScale = myMath::Vector2D(-0.98f, -0.98f);
                                ecsCoordinator.addComponent<PhysicsComponent>(selectedEntityID, physics);
                            }
                            logicSystemRef->assignBehaviour(selectedEntityID, std::make_shared<EnemyBehaviour>());
                        }
                        else if (i == 2) { // Pump
                            logicSystemRef->assignBehaviour(selectedEntityID, std::make_shared<EffectPumpBehaviour>());
                        }
                        else if (i == 3) { // Exit
                            logicSystemRef->assignBehaviour(selectedEntityID, std::make_shared<ExitBehaviour>());
                        }
                        else if (i == 4) { // Collectable
                            logicSystemRef->assignBehaviour(selectedEntityID, std::make_shared<CollectableBehaviour>());
                        }
                        else if (i == 5) { // Player
                            // Check if player already exists
                            bool playerExist = false;
                            for (auto& entity : ecsCoordinator.getAllLiveEntities()) {
                                if ((int)entity != selectedEntityID && ecsCoordinator.hasComponent<PlayerComponent>(entity)) {
                                    playerExist = true;
                                    break;
                                }
                            }

                            if (!playerExist) {
                                if (!ecsCoordinator.hasComponent<PhysicsComponent>(selectedEntityID)) {
                                    PhysicsComponent physics;
                                    physics.gravityScale = myMath::Vector2D(9.8f, 9.8f);
                                    physics.mass = 1.5f;
                                    physics.dampening = 0.9f;
                                    physics.maxVelocity = 200.0f;
                                    physics.force = Force(myMath::Vector2D(0.0f, 0.0f), 10.0f);
                                    physics.maxAccumulatedForce = 40.0f;
                                    ecsCoordinator.addComponent<PhysicsComponent>(selectedEntityID, physics);
                                }

                                if (!ecsCoordinator.hasComponent<PlayerComponent>(selectedEntityID)) {
                                    PlayerComponent player;
                                    player.isPlayer = true;
                                    ecsCoordinator.addComponent<PlayerComponent>(selectedEntityID, player);
                                }

                                logicSystemRef->assignBehaviour(selectedEntityID, std::make_shared<PlayerBehaviour>());
                            }
                            else {
                                ImGui::OpenPopup("Player Warning");
                            }
                        }
                        else if (i == 6) { // Platform
                            if (!ecsCoordinator.hasComponent<ClosestPlatform>(selectedEntityID)) {
                                ClosestPlatform platform;
                                platform.isClosest = false;
                                ecsCoordinator.addComponent<ClosestPlatform>(selectedEntityID, platform);
                            }
                            logicSystemRef->assignBehaviour(selectedEntityID, std::make_shared<PlatformBehaviour>());
                        }
                        else if (i == 7) { // Button
                            if (!ecsCoordinator.hasComponent<ButtonComponent>(selectedEntityID)) {
                                ButtonComponent button;
                                auto& transform = ecsCoordinator.getComponent<TransformComponent>(selectedEntityID);
                                button.originalScale = transform.scale;
                                button.hoveredScale = myMath::Vector2D(transform.scale.GetX() * 1.1f, transform.scale.GetY() * 1.1f);
                                button.isButton = true;
                                ecsCoordinator.addComponent<ButtonComponent>(selectedEntityID, button);
                            }
                            logicSystemRef->assignBehaviour(selectedEntityID, std::make_shared<MouseBehaviour>());
                        }
                        else if (i == 8) { // Filter
                            if (!ecsCoordinator.hasComponent<FilterComponent>(selectedEntityID)) {
                                FilterComponent filter;
                                filter.isFilter = true;
                                filter.isFilterClogged = false;
                                ecsCoordinator.addComponent<FilterComponent>(selectedEntityID, filter);
                            }
                            logicSystemRef->assignBehaviour(selectedEntityID, std::make_shared<FilterBehaviour>());
                        }
                        else if (i == 9) { // MovPlatform
                            if (!ecsCoordinator.hasComponent<MovPlatformComponent>(selectedEntityID)) {
                                MovPlatformComponent movPlatform;
                                auto& transform = ecsCoordinator.getComponent<TransformComponent>(selectedEntityID);
                                movPlatform.startPos = transform.position;
                                movPlatform.direction = myMath::Vector2D(1.0f, 0.0f);
                                movPlatform.speed = 50.0f;
                                movPlatform.maxDistance = 200.0f;
                                ecsCoordinator.addComponent<MovPlatformComponent>(selectedEntityID, movPlatform);
                            }
                            logicSystemRef->assignBehaviour(selectedEntityID, std::make_shared<MovPlatformBehaviour>());
                        }

                        currentItem = i;
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::SameLine();
            ImGui::Text("Behavior");
        }
    }

    // Entity ID and texture editing
    ImGui::Separator();

    if (assetsManager.checkIfAssetListChanged())
    {
        assetNames->clear();
        for (auto& asset : assetsManager.getAssetList())
        {
            assetNames->push_back(asset);
        }
        assetsManager.setAssetListChanged(false);
    }

    // Add a static string to store the current selected item
    static char selectedTexture[256] = "Select a texture...";
    strcpy_s(selectedTexture, sizeof(selectedTexture), ecsCoordinator.getTextureID(selectedEntityID).c_str());

    // Get the latest texture list directly from the assets manager
    auto& textureList = assetsManager.getTextureList();

    // Create the texture dropdown box for the entity
    ImGui::SetNextItemWidth(200);
    if (ImGui::BeginCombo("##TextureDropdown", selectedTexture))
    {
        for (const auto& texturePair : textureList)
        {
            const std::string& asset = texturePair.first;
            bool is_selected = (strcmp(selectedTexture, asset.c_str()) == 0);
            if (ImGui::Selectable(asset.c_str(), is_selected))
            {
                strcpy_s(selectedTexture, asset.c_str());
                ecsCoordinator.setTextureID(selectedEntityID, selectedTexture);
            }

            if (is_selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    ImGui::SameLine();
    ImGui::Text("Texture");

    // Layer selection
    int originalLayer = layerManager.getEntityLayer(selectedEntityID);
    selectedLayer = originalLayer;

    ImGui::RadioButton("0", &selectedLayer, 0); ImGui::SameLine();
    ImGui::RadioButton("1", &selectedLayer, 1); ImGui::SameLine();
    ImGui::RadioButton("2", &selectedLayer, 2); ImGui::SameLine();
    ImGui::RadioButton("3", &selectedLayer, 3);
    ImGui::SameLine();
    ImGui::Text("Layer");

    if (selectedLayer != originalLayer) {
        layerManager.shiftEntityToLayer(originalLayer, selectedLayer, selectedEntityID);
    }

    // Entity ID editing
    static bool showIDPopup = false;
    static char idBuffer[256];

    if (ImGui::Button("Edit ID")) {
        showIDPopup = true;
        strncpy_s(idBuffer, entityID.c_str(), sizeof(idBuffer));
        ImGui::OpenPopup("Edit Entity ID");
    }

    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Edit Entity ID", &showIDPopup, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::InputText("New ID", idBuffer, sizeof(idBuffer));

        if (ImGui::Button("Apply")) {
            ecs.setEntityID(selectedEntityID, std::string(idBuffer));
            showIDPopup = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel")) {
            showIDPopup = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    ImGui::SameLine();

    // Delete entity button
    if (ImGui::Button("Delete Entity")) {
        ImGui::OpenPopup("Delete Entity?");
    }

    // Delete confirmation popup
    if (ImGui::BeginPopupModal("Delete Entity?", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Are you sure you want to delete this entity?");
        ImGui::Separator();

        if (ImGui::Button("Delete", ImVec2(120, 0))) {
            ecsCoordinator.destroyEntity(selectedEntityID);
            selectedEntityID = -1;
            draggedEntityID = -1;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
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

void Inspector::Cleanup() {
	memset(textBuffer, 0, MAXTEXTSIZE);
	delete overlappingEntities;
	overlappingEntities = nullptr;
	delete assetNames;
	assetNames = nullptr;
	delete filledCells;
	filledCells = nullptr;
}

Entity Inspector::CloneEntity(Entity sourceEntity) {
	static int copyCounter = 1;

    Entity newEntity = ecsCoordinator.createEntity();
    
    // Copy the entity ID with a suffix to avoid duplicates
    std::string originalID = ecsCoordinator.getEntityID(sourceEntity);
	std::string newID = originalID + "_copy" + std::to_string(copyCounter);
	copyCounter++;
    ecsCoordinator.setEntityID(newEntity, newID);
    
    // Copy the texture ID
    std::string textureID = ecsCoordinator.getTextureID(sourceEntity);
    ecsCoordinator.setTextureID(newEntity, textureID);
    
    // Add the entity to the same layer as the original
    int layer = layerManager.getEntityLayer(sourceEntity);
    layerManager.addEntityToLayer(layer, newEntity);
    
    // Copy TransformComponent (required for all entities)
    if (ecsCoordinator.hasComponent<TransformComponent>(sourceEntity)) {
        TransformComponent transform = ecsCoordinator.getComponent<TransformComponent>(sourceEntity);
        ecsCoordinator.addComponent(newEntity, transform);
    }
    
    // Copy other components as needed
    auto logicSystemRef = ecsCoordinator.getSpecificSystem<LogicSystemECS>();
    
    // Copy AABBComponent if exists
    if (ecsCoordinator.hasComponent<AABBComponent>(sourceEntity)) {
        AABBComponent aabb = ecsCoordinator.getComponent<AABBComponent>(sourceEntity);
        ecsCoordinator.addComponent(newEntity, aabb);
    }
    
    // Copy PhysicsComponent if exists
    if (ecsCoordinator.hasComponent<PhysicsComponent>(sourceEntity)) {
        PhysicsComponent physics = ecsCoordinator.getComponent<PhysicsComponent>(sourceEntity);
        ecsCoordinator.addComponent(newEntity, physics);
    }
    
    // Copy FontComponent if exists
    if (ecsCoordinator.hasComponent<FontComponent>(sourceEntity)) {
        FontComponent font = ecsCoordinator.getComponent<FontComponent>(sourceEntity);
        ecsCoordinator.addComponent(newEntity, font);
    }
    
    // Copy AnimationComponent if exists
    if (ecsCoordinator.hasComponent<AnimationComponent>(sourceEntity)) {
        AnimationComponent animation = ecsCoordinator.getComponent<AnimationComponent>(sourceEntity);
        ecsCoordinator.addComponent(newEntity, animation);
    }
    
    // Copy CollectableComponent if exists
    if (ecsCoordinator.hasComponent<CollectableComponent>(sourceEntity)) {
        CollectableComponent collectable = ecsCoordinator.getComponent<CollectableComponent>(sourceEntity);
        ecsCoordinator.addComponent(newEntity, collectable);
        GLFWFunctions::collectableCount++;
    }
    
    // Copy PumpComponent if exists
    if (ecsCoordinator.hasComponent<PumpComponent>(sourceEntity)) {
        PumpComponent pump = ecsCoordinator.getComponent<PumpComponent>(sourceEntity);
        ecsCoordinator.addComponent(newEntity, pump);
    }
    
    // Copy ExitComponent if exists
    if (ecsCoordinator.hasComponent<ExitComponent>(sourceEntity)) {
        ExitComponent exit = ecsCoordinator.getComponent<ExitComponent>(sourceEntity);
        ecsCoordinator.addComponent(newEntity, exit);
    }
    
    // Copy FilterComponent if exists
    if (ecsCoordinator.hasComponent<FilterComponent>(sourceEntity)) {
        FilterComponent filter = ecsCoordinator.getComponent<FilterComponent>(sourceEntity);
        ecsCoordinator.addComponent(newEntity, filter);
    }
    
    // Copy ButtonComponent if exists
    if (ecsCoordinator.hasComponent<ButtonComponent>(sourceEntity)) {
        ButtonComponent button = ecsCoordinator.getComponent<ButtonComponent>(sourceEntity);
        ecsCoordinator.addComponent(newEntity, button);
    }
    
    // Copy UIComponent if exists
    if (ecsCoordinator.hasComponent<UIComponent>(sourceEntity)) {
        UIComponent ui = ecsCoordinator.getComponent<UIComponent>(sourceEntity);
        ecsCoordinator.addComponent(newEntity, ui);
    }
    
    // Copy BehaviourComponent and assign the same behaviour type
    if (ecsCoordinator.hasComponent<BehaviourComponent>(sourceEntity)) {
        BehaviourComponent behaviour = ecsCoordinator.getComponent<BehaviourComponent>(sourceEntity);
        ecsCoordinator.addComponent(newEntity, behaviour);
        
        // Assign the same behaviour implementation
        if (logicSystemRef->hasBehaviour<EnemyBehaviour>(sourceEntity)) {
            logicSystemRef->assignBehaviour(newEntity, std::make_shared<EnemyBehaviour>());
        } else if (logicSystemRef->hasBehaviour<EffectPumpBehaviour>(sourceEntity)) {
            logicSystemRef->assignBehaviour(newEntity, std::make_shared<EffectPumpBehaviour>());
        } else if (logicSystemRef->hasBehaviour<ExitBehaviour>(sourceEntity)) {
            logicSystemRef->assignBehaviour(newEntity, std::make_shared<ExitBehaviour>());
        } else if (logicSystemRef->hasBehaviour<CollectableBehaviour>(sourceEntity)) {
            logicSystemRef->assignBehaviour(newEntity, std::make_shared<CollectableBehaviour>());
        } else if (logicSystemRef->hasBehaviour<PlatformBehaviour>(sourceEntity)) {
            logicSystemRef->assignBehaviour(newEntity, std::make_shared<PlatformBehaviour>());
        } else if (logicSystemRef->hasBehaviour<MouseBehaviour>(sourceEntity)) {
            logicSystemRef->assignBehaviour(newEntity, std::make_shared<MouseBehaviour>());
        } else if (logicSystemRef->hasBehaviour<FilterBehaviour>(sourceEntity)) {
            logicSystemRef->assignBehaviour(newEntity, std::make_shared<FilterBehaviour>());
        } else if (logicSystemRef->hasBehaviour<MovPlatformBehaviour>(sourceEntity)) {
            logicSystemRef->assignBehaviour(newEntity, std::make_shared<MovPlatformBehaviour>());
        }
        // Don't clone Player component - there should only be one player
    }
    
    return newEntity;
}