#include "CameraSystem2D.h"
#include "GlobalCoordinator.h"

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
    m_ViewMatrix = myMath::Matrix3x3(1.0f, 0.0f, 0.0f, 
                                     0.0f, 1.0f, 0.0f, 
                                     0.0f, 0.0f, 1.0f); // Identity matrix

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

        //translationMatrix.SetMatrixValue(2, 0, -m_CameraPosition.GetX());
        //translationMatrix.SetMatrixValue(2, 1, -m_CameraPosition.GetY());
        
    }
  //  else {
		//// Update the camera position based on user input or other logic
  ////      if (GLFWFunctions::move_up_flag)    { m_CameraPosition.SetY(m_CameraPosition.GetY() + (20 * GLFWFunctions::delta_time)); }
  ////      if (GLFWFunctions::move_down_flag)  { m_CameraPosition.SetY(m_CameraPosition.GetY() - (20 * GLFWFunctions::delta_time)); }
  ////      if (GLFWFunctions::move_left_flag)  { m_CameraPosition.SetX(m_CameraPosition.GetX() - (20 * GLFWFunctions::delta_time)); }
  ////      if (GLFWFunctions::move_right_flag) { m_CameraPosition.SetX(m_CameraPosition.GetX() + (20 * GLFWFunctions::delta_time)); }
  ////          
  ////      //------------------------------------------------------------//
		////if (GLFWFunctions::camera_zoom_in_flag)
		////	m_CameraZoom += 0.1f * GLFWFunctions::delta_time;
		////if (GLFWFunctions::camera_zoom_out_flag)
		////	m_CameraZoom -= 0.1f * GLFWFunctions::delta_time;
		//////------------------------------------------------------------//
		////if (GLFWFunctions::camera_rotate_left_flag)
		////	m_CameraRotation += 0.1f * GLFWFunctions::delta_time;
		////if (GLFWFunctions::camera_rotate_right_flag)
		////	m_CameraRotation -= 0.1f * GLFWFunctions::delta_time;
  //      
  //      // Update the view matrix based on the camera's position, rotation, and zoom
  //      //translationMatrix[2][0] = m_CameraPosition.x;
  //      //translationMatrix[2][1] = m_CameraPosition.y;        
  //      translationMatrix.SetMatrixValue(2, 0, m_CameraPosition.GetX());
  //      translationMatrix.SetMatrixValue(2, 1, m_CameraPosition.GetY());
  //  }

    //// Update the view matrix based on the camera's position, rotation, and zoom
    //glm::mat3 translationMatrix = glm::mat3(1.0f);
    //translationMatrix[2][0] = -m_CameraPosition.x;
    //translationMatrix[2][1] = -m_CameraPosition.y;
    translationMatrix.SetMatrixValue(2, 0, -m_CameraPosition.GetX());
    translationMatrix.SetMatrixValue(2, 1, -m_CameraPosition.GetY());

    //glm::mat3 rotationMatrix = glm::mat3(1.0f);
    myMath::Matrix3x3 rotationMatrix(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    float cosTheta = cos(m_CameraRotation);
    float sinTheta = sin(m_CameraRotation);
    //rotationMatrix[0][0] = cosTheta;
    //rotationMatrix[0][1] = sinTheta;
    //rotationMatrix[1][0] = -sinTheta;
    //rotationMatrix[1][1] = cosTheta;
    rotationMatrix.SetMatrixValue(0, 0,  cosTheta);
    rotationMatrix.SetMatrixValue(0, 1,  sinTheta);
    rotationMatrix.SetMatrixValue(1, 0, -sinTheta);
    rotationMatrix.SetMatrixValue(1, 1,  cosTheta);


    //glm::mat3 scaleMatrix = glm::mat3(1.0f);
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
    return SystemType::CameraType; // Assuming SystemType has a Camera enum value
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

bool CameraSystem2D::checkLockedComponent() {
    return ((m_LockedComponent) ? true : false);
}