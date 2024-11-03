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
    void Update(float deltaTime, GLboolean isAnimated);
    void Render(float deltaTime);
    void cleanup() override;
    SystemType getSystem() override; //For perfomance viewer

    glm::mat3x3 UpdateObject(GLdouble deltaTime, myMath::Vector2D objPos, myMath::Vector2D objScale, myMath::Vector2D objOri, glm::mat3 viewMat);
    void DrawObject(DrawMode mode, const GLuint texture, glm::mat3 xform);

    //Shader* GetShader() const{ return m_Shader.get(); }
    //Shader* GetShader2() const{ return m_Shader2.get(); }
    //GLuint GetTexture() const { return m_Texture; }
    //GLuint GetTexture2() const { return m_Texture2; }
    //GLuint GetTexture3() const { return m_Texture3; }
    GLuint GetVAO() const { return m_VAO; }
    void SetCurrentAction(int actionRow);

    void loadShaderAssets() const;
    void loadTextureAssets() const;

    struct GLViewport {
        GLint x, y;
        GLsizei width, height;
    };

    static std::vector<GLViewport> vps; // container for viewports

    struct GLObject {
        glm::vec2 orientation;
        glm::vec2 scaling;
        glm::vec2 position;
        glm::vec3 color;
        glm::mat3 mdl_xform;
        glm::mat3 mdl_to_ndc_xform;
        GLboolean is_animated;

        void init(glm::vec2 orientation, glm::vec2 scaling, glm::vec2 position);
        void update(GLdouble time_per_frame);
        void draw(Shader* shader, const GLuint vao, const GLuint tex) const;

    };
    void drawDebugAABB(AABBComponent aabb, glm::mat3 viewMat);

private:
    GLuint m_VAO;
    GLuint m_VBO;       // VBO for vertex positions
    GLuint m_UVBO;      // Separate VBO for texture coordinates (UVs)
    GLuint m_EBO;       // Element Buffer Object
    GLuint m_Texture, m_Texture2, m_Texture3;
    std::unique_ptr<Shader> m_Shader, m_Shader2;
    std::unique_ptr<AnimationData> m_AnimationData;  // Pointer to the AnimationData instance
    std::unique_ptr<AnimationData> idleAnimation;  // Pointer to the AnimationData instance

    void ReleaseResources();
};

