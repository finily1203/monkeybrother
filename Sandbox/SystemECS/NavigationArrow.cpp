/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Liu YaoTing (yaoting.liu)
@team:   MonkeHood
@course: CSD2401
@file:   NavigationArrow.cpp
@brief:  This source file includes the implementation of the NavigationArrow
		 that logicSystemECS uses to handle the behaviour of the navigation arrow entity.

		 Liu YaoTing (yaoting.liu): defined the functions of NavigationArrow class
							   100%
*//*___________________________________________________________________________-*/
#include "NavigationArrow.h"
#include "GlobalCoordinator.h"
#include "PlayerComponent.h"
#include "CollectableComponent.h"
#include <iostream> // For debugging

#define M_PI 3.14159265358979323846
#define ORBIT_RADIUS 150.0f
std::unordered_map<Entity, Entity>* NavigationArrow::targetToArrowMap;
Entity NavigationArrow::playerEntity = 0;
bool NavigationArrow::initialized = false;

void NavigationArrow::Initialize() {
    // Reset static variables
    playerEntity = 0;

    if (!targetToArrowMap) {
        targetToArrowMap = new std::unordered_map<Entity, Entity>();
    }

    targetToArrowMap->clear();
    initialized = true;

    // Find the player entity
    for (auto entity : ecsCoordinator.getAllLiveEntities()) {
        if (ecsCoordinator.hasComponent<PlayerComponent>(entity)) {
            playerEntity = entity;
            break;
        }
    }

    // Note: It's okay if we don't find the player yet - we'll keep trying in Update
}

void NavigationArrow::Update() {
    if (!initialized) {
        Initialize();
    }

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
    for (auto it = targetToArrowMap->begin(); it != targetToArrowMap->end(); ) {
        Entity targetEntity = it->first;
        Entity arrowEntity = it->second;

        // Check if target and arrow still exist
        if (!ecsCoordinator.entityExists(targetEntity) || !ecsCoordinator.entityExists(arrowEntity)) {
            // If arrow still exists but target doesn't, hide the arrow rather than destroying it
            if (ecsCoordinator.entityExists(arrowEntity)) {
                auto& navComp = ecsCoordinator.getComponent<NavigationComponent>(arrowEntity);
                navComp.isVisible = false;
            }
            // Remove from map
            it = targetToArrowMap->erase(it);
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
    if (!initialized || !ecsCoordinator.entityExists(targetEntity)) {
        return;
    }

    // Don't create duplicate arrows
    if (targetToArrowMap->find(targetEntity) != targetToArrowMap->end()) {
        // If arrow already exists for this target, make sure it's visible and update position
        Entity arrowEntity = (*targetToArrowMap)[targetEntity];
        if (ecsCoordinator.entityExists(arrowEntity)) {
            auto& navComp = ecsCoordinator.getComponent<NavigationComponent>(arrowEntity);
            navComp.isVisible = true;
            UpdateArrowPositionAndRotation(arrowEntity, targetEntity);
        }
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

    // Check if we have an unused arrow in our pool
    Entity arrowEntity = 0;
    for (auto entity : ecsCoordinator.getAllLiveEntities()) {
        if (ecsCoordinator.hasComponent<NavigationComponent>(entity) &&
            ecsCoordinator.getEntityID(entity) == "nav_arrow") {

            // Check if this arrow is not already in our map
            bool inUse = false;
            for (const auto& pair : *targetToArrowMap) {
                if (pair.second == entity) {
                    inUse = true;
                    break;
                }
            }

            if (!inUse) {
                arrowEntity = entity;
                auto& navComp = ecsCoordinator.getComponent<NavigationComponent>(entity);
                navComp.isVisible = true;
                break;
            }
        }
    }

    // If no unused arrow found, create a new one
    if (arrowEntity == 0) {
        arrowEntity = ecsCoordinator.createEntity();

        // Set up components for the arrow
        TransformComponent transform;
        transform.scale.SetX(ARROW_SIZE);
        transform.scale.SetY(ARROW_SIZE);
        ecsCoordinator.addComponent(arrowEntity, transform);

        // Add navigation component
        NavigationComponent navComp;
        navComp.isNavigation = true;
        navComp.isVisible = true;
        ecsCoordinator.addComponent(arrowEntity, navComp);

        // Set texture and entity ID
        ecsCoordinator.setTextureID(arrowEntity, "nav_arrow");
        ecsCoordinator.setEntityID(arrowEntity, "nav_arrow");

        // Add to highest layer for UI elements
        int topLayer = layerManager.getLayerCount() - 1;
        layerManager.addEntityToLayer(topLayer, arrowEntity);
    }

    // Store the mapping
    (*targetToArrowMap)[targetEntity] = arrowEntity;

    // Initial position update
    UpdateArrowPositionAndRotation(arrowEntity, targetEntity);
}

void NavigationArrow::RemoveNavigationArrow(Entity targetEntity) {
    if (!initialized) {
        return;
    }

    auto it = targetToArrowMap->find(targetEntity);
    if (it != targetToArrowMap->end()) {
        Entity arrowEntity = it->second;
        if (ecsCoordinator.entityExists(arrowEntity)) {
            // Instead of destroying, just hide it for potential reuse
            auto& navComp = ecsCoordinator.getComponent<NavigationComponent>(arrowEntity);
            navComp.isVisible = false;
        }
        targetToArrowMap->erase(it);
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

    // Calculate angle from player to target
    float angle = CalculateAngleToTarget(playerPos, targetPos);

    // Position arrow in orbit around the player in the direction of the target
    float orbitRadius = ORBIT_RADIUS; // Orbit radius around player
    float radians = static_cast<float>(angle * M_PI / 180.0f);

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
    float angle = static_cast<float>(std::atan2(dy, dx) * 180.0f / M_PI);

    return angle;
}

void NavigationArrow::Reset() {
    if (!initialized || !targetToArrowMap) {
        return;
    }

    // Hide all arrows but keep them for reuse
    for (const auto& pair : *targetToArrowMap) {
        Entity arrowEntity = pair.second;
        if (ecsCoordinator.entityExists(arrowEntity)) {
            auto& navComp = ecsCoordinator.getComponent<NavigationComponent>(arrowEntity);
            navComp.isVisible = false;
        }
    }

    targetToArrowMap->clear();
}

void NavigationArrow::Cleanup() {
    if (!initialized || !targetToArrowMap) {
        return;
    }

    // Now we actually destroy all arrow entities
    for (const auto& pair : *targetToArrowMap) {
        Entity arrowEntity = pair.second;
        if (ecsCoordinator.entityExists(arrowEntity)) {
            ecsCoordinator.destroyEntity(arrowEntity);
        }
    }

    // Also destroy any other unused navigation arrows
    for (auto entity : ecsCoordinator.getAllLiveEntities()) {
        if (ecsCoordinator.hasComponent<NavigationComponent>(entity) &&
            ecsCoordinator.getEntityID(entity) == "nav_arrow") {
            ecsCoordinator.destroyEntity(entity);
        }
    }

    // Clear the map and reset variables
    delete targetToArrowMap;
    targetToArrowMap = nullptr;
    playerEntity = 0;
    initialized = false;
}