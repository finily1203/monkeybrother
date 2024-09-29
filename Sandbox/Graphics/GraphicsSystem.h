#pragma once

#include <GL/glew.h>
#include <memory>
#include "Shader.h"
#include "AnimationData.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader;


class GraphicsSystem {
public:
    GraphicsSystem();
    ~GraphicsSystem();



    void Initialize();
    void Update(float deltaTime, GLboolean isAnimated);
    void Render(float deltaTime);
    void Cleanup();

    Shader* GetShader() const;
    GLuint GetTexture() const { return m_Texture; }
    GLuint GetTexture2() const { return m_Texture2; }
    GLuint GetTexture3() const { return m_Texture3; }
    GLuint GetVAO() const { return m_VAO; }
    void SetCurrentAction(int actionRow);

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
private:
    GLuint m_VAO;
    GLuint m_VBO;       // VBO for vertex positions
    GLuint m_UVBO;      // Separate VBO for texture coordinates (UVs)
    GLuint m_EBO;       // Element Buffer Object
    GLuint m_Texture, m_Texture2, m_Texture3;
    std::unique_ptr<Shader> m_Shader;
    std::unique_ptr<AnimationData> m_AnimationData;  // Pointer to the AnimationData instance
    std::unique_ptr<AnimationData> idleAnimation;  // Pointer to the AnimationData instance

    void ReleaseResources();
};

