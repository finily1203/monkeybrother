/*
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Liu YaoTing (yaoting.liu), Javier Chua (javierjunliang.chua)
@team   :  MonkeHood
@course :  CSD2401
@file   :  GraphicsSystem.h
@brief  :  This file contains the declaration of the GraphicsSystem class, which
           is responsible for rendering the game objects.

* Liu YaoTing (yaoting.liu) :
* 	    - Further helped to implement the basic structure of the GraphicsSystem class that inherits from GameSystems.
*       - Added the basic structure of the GLObject struct that is used to store the
          orientation, scaling, position, color, model transformation, and model-to-NDC transformation
          of the game objects. ( For Milestione 1 test only)
*
* Javier Chua (javierjunliang.chua) :
*       - Implemented the the basic structure of the GraphicsSystem class that inherits from GameSystems,
*         such as the constructor, destructor, initialise, update, and cleanup functions,
*         as well as the data members.
*
* File Contributions: Liu YaoTing (50%), Javier Chua (50%)
*
/*_ _ _ _ ________________________________________________________________________________-\*/

#pragma once
#include "EngineDefinitions.h"
#include <GL/glew.h>
#include <memory>
#include "Shader.h"
#include "AnimationData.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Systems.h"
#include <string>
#include "GlfwFunctions.h"
#include "AABBComponent.h"
#include "vector2D.h"
#include "vector3D.h"
#include "matrix3x3.h"
#include "TransformComponent.h"
#include "AnimationComponent.h"

class GraphicsSystem : public GameSystems
{
public:
    GraphicsSystem();
    ~GraphicsSystem();

    enum class DrawMode {
        COLOR,
        TEXTURE
    };

    void initialise() override;
    void update() override;
    void Update(float deltaTime, const AnimationComponent& animation);
    void Render(float deltaTime);
    void cleanup() override;
    SystemType getSystem() override; //For perfomance viewer

    myMath::Matrix3x3 UpdateObject(myMath::Vector2D objPos, myMath::Vector2D objScale, myMath::Vector2D objOri, myMath::Matrix3x3 viewMat);
    void DrawObject(DrawMode mode, const GLuint texture, const myMath::Matrix3x3& xform, const std::vector<glm::vec2>& uvCoords);

    GLuint GetVAO() const { return m_VAO; }

    struct GLViewport {
        GLint x, y;
        GLsizei width, height;
    };

    std::vector<GLViewport>* vps; // container for viewports
    void drawDebugOBB(TransformComponent transform, myMath::Matrix3x3 viewMatrix);
    void drawDebugCircle(const TransformComponent transform, const myMath::Matrix3x3 viewMatrix);

private:
    GLuint m_VAO;
    GLuint m_VBO;       // VBO for vertex positions
    GLuint m_UVBO;      // Separate VBO for texture coordinates (UVs)
    GLuint m_EBO;       // Element Buffer Object
    GLuint m_Texture, m_Texture2, m_Texture3;
    GLboolean is_animated;
    std::unique_ptr<Shader> m_Shader, m_Shader2;


    void ReleaseResources();
};

