/*
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Liu YaoTing (yaoting.liu), Javier Chua (javierjunliang.chua)
@team   :  MonkeHood
@course :  CSD2401
@file   :  CameraSystem2D.cpp
@brief  :  This file contains the implementation of the CameraSystem2D class. It is 
           responsible for managing the camera's position,
           rotation, and zoom, as well as updating the view matrix based on these values. 
           The CameraSystem2D class also provides functionality to lock the camera to an
           entity's position and orientation.
		

 File Contributions: Liu YaoTing (100%)

/*_______________________________________________________________________________________________________________*/



#include "CameraSystem2D.h"
#include "GlobalCoordinator.h"

#define M_PI   3.14159265358979323846264338327950288f
CameraSystem2D::CameraSystem2D()
	: m_CameraPosition(0.0f, 0.0f), m_CameraRotation(0.0f), m_CameraZoom(1.0f), ecsCoordinator(ecsCoordinator), m_LockedComponent(nullptr), m_ViewMatrix(), m_ProjectionMatrix()
{
    // Constructor implementation
}

CameraSystem2D::~CameraSystem2D()
{
    // Destructor implementation
	cleanup();
}

void CameraSystem2D::initialise()
{
    // Set the initial view and projection matrices
    m_ViewMatrix = myMath::Matrix3x3(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f); // Identity matrix

    // Set initial camera position, rotation, and zoom if needed
    m_CameraPosition = myMath::Vector2D(0.0f, 0.0f);
    m_CameraRotation = 0.0f;
    m_CameraZoom = 1.0f;
}

void CameraSystem2D::update()
{
    myMath::Matrix3x3 translationMatrix(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    // If the camera is locked to an entity, update the camera position based on the entity's position
    if (m_LockedComponent && GLFWFunctions::allow_camera_movement == false)
    {
		myMath::Vector2D entityPosition = { m_LockedComponent->position.GetX(), m_LockedComponent->position.GetY() };
        setCameraPosition(entityPosition);
        //if (GLFWFunctions::keyState[Key::Z])
        //    m_CameraZoom += 0.1f * GLFWFunctions::delta_time;
        //if (GLFWFunctions::keyState[Key::X])
        //    m_CameraZoom -= 0.1f * GLFWFunctions::delta_time;
        //// Update the view matrix based on the camera's position, rotation, and zoom
       
        //translationMatrix.SetMatrixValue(2, 0, -m_CameraPosition.GetX());
        //translationMatrix.SetMatrixValue(2, 1, -m_CameraPosition.GetY());
    }
    else {
		// Currently fixed camera position
    }
  
    translationMatrix.SetMatrixValue(2, 0, -m_CameraPosition.GetX());
    translationMatrix.SetMatrixValue(2, 1, -m_CameraPosition.GetY());

    myMath::Matrix3x3 rotationMatrix(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    float cosTheta = 0.0f;
    float sinTheta = 0.0f;
    //std::cout << "m_LockedComponent->orientation.GetX(): " << m_LockedComponent->orientation.GetX() << std::endl;
    if (m_LockedComponent && GLFWFunctions::allow_camera_movement == false) {
        cosTheta = cos(-m_LockedComponent->orientation.GetX() * M_PI / 180.0f);
        sinTheta = sin(-m_LockedComponent->orientation.GetX() * M_PI / 180.0f);
    }
    else {
		cosTheta = cos(-m_CameraRotation);
		sinTheta = sin(-m_CameraRotation);
	}
    
    //rotationMatrix[0][0] = cosTheta;
    //rotationMatrix[0][1] = sinTheta;
    //rotationMatrix[1][0] = -sinTheta;
    //rotationMatrix[1][1] = cosTheta;
    rotationMatrix.SetMatrixValue(0, 0, cosTheta);
    rotationMatrix.SetMatrixValue(0, 1, sinTheta);
    rotationMatrix.SetMatrixValue(1, 0, -sinTheta);
    rotationMatrix.SetMatrixValue(1, 1, cosTheta);

    myMath::Matrix3x3 scaleMatrix(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    //scaleMatrix[0][0] = m_CameraZoom;
    //scaleMatrix[1][1] = m_CameraZoom;
    scaleMatrix.SetMatrixValue(0, 0, m_CameraZoom);
    scaleMatrix.SetMatrixValue(1, 1, m_CameraZoom);

    m_ViewMatrix = translationMatrix * rotationMatrix * scaleMatrix;
}

void CameraSystem2D::cleanup()
{
    // Cleanup code
}

SystemType CameraSystem2D::getSystem()
{
    return CameraType; // Assuming SystemType has a Camera enum value
}

void CameraSystem2D::setViewMatrix(const myMath::Matrix3x3& viewMatrix)
{
    m_ViewMatrix = viewMatrix;
}

myMath::Matrix3x3 CameraSystem2D::getViewMatrix() const
{
    return m_ViewMatrix;
}

void CameraSystem2D::setProjectionMatrix(const myMath::Matrix3x3& projectionMatrix)
{
    m_ProjectionMatrix = projectionMatrix;
}

myMath::Matrix3x3 CameraSystem2D::getProjectionMatrix() const
{
    return m_ProjectionMatrix;
}

void CameraSystem2D::setCameraPosition(const myMath::Vector2D& position)
{
    m_CameraPosition = position;
}

myMath::Vector2D CameraSystem2D::getCameraPosition() const
{
    return m_CameraPosition;
}

void CameraSystem2D::setCameraRotation(const GLfloat& rotation)
{
    m_CameraRotation = rotation;
}

GLfloat CameraSystem2D::getCameraRotation() const
{
    return m_CameraRotation;
}

void CameraSystem2D::setCameraZoom(const GLfloat& zoom)
{
    m_CameraZoom = zoom;
}

GLfloat CameraSystem2D::getCameraZoom() const
{
    return m_CameraZoom;
}

void CameraSystem2D::lockToComponent(const TransformComponent& component)
{
    m_LockedComponent = &component;
}

bool CameraSystem2D::checkLockedComponent() const
{
    return (m_LockedComponent ? true : false);
}