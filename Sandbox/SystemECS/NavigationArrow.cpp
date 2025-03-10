// NavigationArrow.cpp
#include "NavigationArrow.h"
#include "GlobalCoordinator.h"
#include "PlayerComponent.h"
#include "CollectableComponent.h"
#include <iostream> // For debugging

#define M_PI 3.14159265358979323846
#define ORBIT_RADIUS 150.0f
std::unordered_map<Entity, Entity> NavigationArrow::targetToArrowMap;
Entity NavigationArrow::playerEntity = 0;

void NavigationArrow::Initialize() {
    // Reset static variables
    playerEntity = 0;
    targetToArrowMap.clear();

    // Find the player entity
    for (auto entity : ecsCoordinator.getAllLiveEntities()) {
        if (ecsCoordinator.hasComponent<PlayerComponent>(entity)) {
            playerEntity = entity;
            break;
        }
    }

    // Note: It's okay if we don't find the player yet - we'll keep trying in Update
}

void NavigationArrow::Update(float deltaTime) {
    // Try to find player entity if we don't have a valid one
    if (playerEntity == 0 || !ecsCoordinator.entityExists(playerEntity)) {
        // Player not found, try to find it
        for (auto entity : ecsCoordinator.getAllLiveEntities()) {
            if (ecsCoordinator.hasComponent<PlayerComponent>(entity)) {
                playerEntity = entity;
                break;
            }
        }

        if (playerEntity == 0) {
            // Still no player, don't update arrows until we have a player
            return;
        }
    }

    // Update all navigation arrows
    std::vector<Entity> targetEntitiesToRemove;

    for (auto it = targetToArrowMap.begin(); it != targetToArrowMap.end(); ) {
        Entity targetEntity = it->first;
        Entity arrowEntity = it->second;

        // Check if target and arrow still exist
        if (!ecsCoordinator.entityExists(targetEntity) || !ecsCoordinator.entityExists(arrowEntity)) {
            // If either doesn't exist, remove the arrow entity if it still exists
            if (ecsCoordinator.entityExists(arrowEntity)) {
                ecsCoordinator.destroyEntity(arrowEntity);
            }
            // Erase from map and get next iterator
            it = targetToArrowMap.erase(it);
        }
        else {
            // Update arrow position and rotation
            UpdateArrowPositionAndRotation(arrowEntity, targetEntity);
            ++it;
        }
    }
}

void NavigationArrow::CreateNavigationArrow(Entity targetEntity) {
    // Check if the target exists
    if (!ecsCoordinator.entityExists(targetEntity)) {
        return;
    }

    // Don't create duplicate arrows
    if (targetToArrowMap.find(targetEntity) != targetToArrowMap.end()) {
        // If arrow already exists for this target, we're done
        return;
    }

    // Make sure we have a valid player entity
    if (playerEntity == 0 || !ecsCoordinator.entityExists(playerEntity)) {
        // Try to find player entity
        for (auto entity : ecsCoordinator.getAllLiveEntities()) {
            if (ecsCoordinator.hasComponent<PlayerComponent>(entity)) {
                playerEntity = entity;
                break;
            }
        }

        if (playerEntity == 0) {
            // Still no player entity, can't create arrows yet
            return;
        }
    }

    // Create a new arrow entity
    Entity arrowEntity = ecsCoordinator.createEntity();

    // Set up components for the arrow
    TransformComponent transform;
    transform.scale.SetX(ARROW_SIZE);
    transform.scale.SetY(ARROW_SIZE);
    ecsCoordinator.addComponent(arrowEntity, transform);

    // Add navigation component
    NavigationComponent navComp;
    navComp.isNavigation = true;
    navComp.isVisible = true; // Start hidden
    ecsCoordinator.addComponent(arrowEntity, navComp);

    // Set texture and entity ID
    ecsCoordinator.setTextureID(arrowEntity, "nav_arrow"); // Use your nav_arrow texture
    ecsCoordinator.setEntityID(arrowEntity, "nav_arrow");

    // Add to highest layer for UI elements
    int topLayer = layerManager.getLayerCount() - 1;
    layerManager.addEntityToLayer(topLayer, arrowEntity);

    // Store the mapping
    targetToArrowMap[targetEntity] = arrowEntity;

    // Initial position update
    UpdateArrowPositionAndRotation(arrowEntity, targetEntity);
}

void NavigationArrow::RemoveNavigationArrow(Entity targetEntity) {
    auto it = targetToArrowMap.find(targetEntity);
    if (it != targetToArrowMap.end()) {
        Entity arrowEntity = it->second;
        if (ecsCoordinator.entityExists(arrowEntity)) {
            ecsCoordinator.destroyEntity(arrowEntity);
        }
        targetToArrowMap.erase(it);
    }
}


void NavigationArrow::UpdateArrowPositionAndRotation(Entity arrowEntity, Entity targetEntity) {
    if (playerEntity == 0 || !ecsCoordinator.entityExists(arrowEntity) || !ecsCoordinator.entityExists(targetEntity)) {
        return;
    }

    // Get player and target positions
    const TransformComponent& playerTransform = ecsCoordinator.getComponent<TransformComponent>(playerEntity);
    const TransformComponent& targetTransform = ecsCoordinator.getComponent<TransformComponent>(targetEntity);

    myMath::Vector2D playerPos = playerTransform.position;
    myMath::Vector2D targetPos = targetTransform.position;

    // Always make arrows visible regardless of whether target is on screen or not
    auto& navComp = ecsCoordinator.getComponent<NavigationComponent>(arrowEntity);
    navComp.isVisible = true;

    // Calculate angle from player to target
    float angle = CalculateAngleToTarget(playerPos, targetPos);

    // Position arrow in orbit around the player in the direction of the target
    float orbitRadius = ORBIT_RADIUS; // Orbit radius around player
    float radians = angle * M_PI / 180.0f;

    // Calculate orbit position
    float arrowX = playerPos.GetX() + orbitRadius * std::cos(radians);
    float arrowY = playerPos.GetY() + orbitRadius * std::sin(radians);

    // Update arrow transform
    auto& arrowTransform = ecsCoordinator.getComponent<TransformComponent>(arrowEntity);
    arrowTransform.position.SetX(arrowX);
    arrowTransform.position.SetY(arrowY);
    arrowTransform.orientation.SetX(angle); // Point toward the target
}

float NavigationArrow::CalculateAngleToTarget(const myMath::Vector2D& playerPos, const myMath::Vector2D& targetPos) {
    // Calculate direction vector from player to target
    float dx = targetPos.GetX() - playerPos.GetX();
    float dy = targetPos.GetY() - playerPos.GetY();

    // Calculate angle in degrees (0 degrees is pointing right)
    float angle = std::atan2(dy, dx) * 180.0f / M_PI;

    return angle;
}

bool NavigationArrow::IsEntityOnScreen(const myMath::Vector2D& entityPos, float entityRadius) {
    // Get camera position and viewport dimensions
    myMath::Vector2D cameraPos = cameraSystem.getCameraPosition();
    float cameraZoom = cameraSystem.getCameraZoom();

    // Calculate screen boundaries
    float screenWidth = GLFWFunctions::windowWidth / cameraZoom;
    float screenHeight = GLFWFunctions::windowHeight / cameraZoom;

    float left = cameraPos.GetX() - (screenWidth / 2.0f);
    float right = cameraPos.GetX() + (screenWidth / 2.0f);
    float top = cameraPos.GetY() + (screenHeight / 2.0f);
    float bottom = cameraPos.GetY() - (screenHeight / 2.0f);

    // Check if the entity is fully outside the screen boundaries
    if (entityPos.GetX() + entityRadius < left ||
        entityPos.GetX() - entityRadius > right ||
        entityPos.GetY() + entityRadius < bottom ||
        entityPos.GetY() - entityRadius > top) {
        return false;
    }

    return true;
}

myMath::Vector2D NavigationArrow::GetScreenEdgePosition(const myMath::Vector2D& playerPos, float angle) {
    // Convert angle to radians
    float radians = angle * M_PI / 180.0f;

    // Calculate direction vector
    float dx = std::cos(radians);
    float dy = std::sin(radians);

    // Instead of positioning at screen edge, we'll position the arrow in orbit around the player
    // Position arrow at a fixed radius around the player
    float x = playerPos.GetX() + (ORBIT_RADIUS * dx);
    float y = playerPos.GetY() + (ORBIT_RADIUS * dy);

    return myMath::Vector2D(x, y);
}

void NavigationArrow::Reset() {
    // Clear all existing navigation arrows
    for (const auto& pair : targetToArrowMap) {
        if (ecsCoordinator.entityExists(pair.second)) {
            ecsCoordinator.destroyEntity(pair.second);
        }
    }

    targetToArrowMap.clear();
    playerEntity = 0;
}