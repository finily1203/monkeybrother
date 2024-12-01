/*
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Liu YaoTing (yaoting.liu), Javier Chua (javierjunliang.chua)
@team   :  MonkeHood
@course :  CSD2401
@file   :  CameraSystem2D.h
@brief  : This file contains the declaration of the CameraSystem2D class. It is
		  responsible for managing the camera's position,
		  rotation, and zoom, as well as updating the view matrix based on these values.
		  The CameraSystem2D class also provides functionality to lock the camera to an
		  entity's position and orientation.


 File Contributions: Liu YaoTing (100%)

/*_______________________________________________________________________________________________________________*/

#pragma once
#include "EngineDefinitions.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "Systems.h"            // Include GameSystems
#include "ECSCoordinator.h"     // Include ECS Coordinator
#include "GlfwFunctions.h"


class CameraSystem2D : public GameSystems
{
public:
	// Constructor
    CameraSystem2D();
	// Destructor
    ~CameraSystem2D();

	// GameSystems methods
    void initialise() override;
    void update() override;
    void cleanup() override;
    SystemType getSystem() override; // For performance viewer

    // Camera-specific methods
    void setViewMatrix(const myMath::Matrix3x3& viewMatrix);
    myMath::Matrix3x3 getViewMatrix() const;
	// Set the projection matrix
    void setProjectionMatrix(const myMath::Matrix3x3& projectionMatrix);
	// Get the projection matrix
    myMath::Matrix3x3 getProjectionMatrix() const;
	// Set the camera position
    void setCameraPosition(const myMath::Vector2D& position);
	// Get the camera position
    myMath::Vector2D getCameraPosition() const;
	// Set the camera rotation
    void setCameraRotation(const GLfloat& rotation);
	// Get the camera rotation
    GLfloat getCameraRotation() const;
	// Set the camera zoom
    void setCameraZoom(const GLfloat& zoom);
	// Get the camera zoom
    GLfloat getCameraZoom() const;

    void lockToComponent(const TransformComponent& component); // Lock camera to an entity
    bool checkLockedComponent() const; // Check if camera is locked to an entity

private:
    myMath::Matrix3x3 m_ViewMatrix;
    myMath::Matrix3x3 m_ProjectionMatrix;
    myMath::Vector2D m_CameraPosition;
    GLfloat m_CameraRotation;
    GLfloat m_CameraZoom;
    const TransformComponent* m_LockedComponent;
    ECSCoordinator& ecsCoordinator; // Reference to the ECS Coordinator
};