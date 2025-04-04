/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Liu YaoTing (yaoting.liu)
@team:   MonkeHood
@course: CSD2401
@file:   NavigationArrow.h
@brief:  This header file includes the declaration of the NavigationArrow
		 that logicSystemECS uses to handle the navigation arrow entity.

         Liu YaoTing (yaoting.liu): defined the functions of NavigationArrow class
                               100%
*//*___________________________________________________________________________-*/
#pragma once
#include "EngineDefinitions.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "EntityManager.h"
#include "TransformComponent.h"
#include "CameraSystem2D.h"
#include "GraphicsSystem.h"
#include "NavigationComponent.h"
#include <vector>
#include <cmath>

class NavigationArrow {
public:
    static void Initialize();
    static void Update();
    static void CreateNavigationArrow(Entity targetEntity);
    static void RemoveNavigationArrow(Entity targetEntity);
    static void Reset();
    static void Cleanup();

    static constexpr float ARROW_OFFSET = 100.0f; // How far from the player to render the arrow
    static constexpr float ARROW_SIZE = 50.0f;    // Size of the arrow

private:
    static std::unordered_map<Entity, Entity>* targetToArrowMap;
    static Entity playerEntity;
    static bool initialized;

    static void UpdateArrowPositionAndRotation(Entity arrowEntity, Entity targetEntity);
    static float CalculateAngleToTarget(const myMath::Vector2D& playerPos, const myMath::Vector2D& targetPos);
};