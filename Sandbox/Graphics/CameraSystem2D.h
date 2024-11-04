#pragma once
#include "EngineDefinitions.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "Systems.h"            // Include GameSystems
#include "ECSCoordinator.h"     // Include ECS Coordinator
#include "GlobalCoordinator.h"  // Include system type enum
#include "GlfwFunctions.h"


class CameraSystem2D : public GameSystems
{
public:
    CameraSystem2D();
    ~CameraSystem2D();

    void initialise() override;
    void update() override;
    void cleanup() override;
    SystemType getSystem() override; // For performance viewer

    // Camera-specific methods
    void setViewMatrix(const glm::mat3& viewMatrix);
    glm::mat3 getViewMatrix() const;

    void setProjectionMatrix(const glm::mat3& projectionMatrix);
    glm::mat3 getProjectionMatrix() const;

    void setCameraPosition(const glm::vec2& position);
    glm::vec2 getCameraPosition() const;

    void setCameraRotation(const GLfloat& rotation);
    GLfloat getCameraRotation() const;

    void setCameraZoom(const GLfloat& zoom);
    GLfloat getCameraZoom() const;

    void lockToComponent(const TransformComponent& component); // Lock camera to an entity

private:
    glm::mat3 m_ViewMatrix;
    glm::mat3 m_ProjectionMatrix;
    glm::vec2 m_CameraPosition;
    GLfloat m_CameraRotation;
    GLfloat m_CameraZoom;
    const TransformComponent* m_LockedComponent;
    ECSCoordinator& ecsCoordinator; // Reference to the ECS Coordinator
};