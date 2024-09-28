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

    GraphicsSystem(const GraphicsSystem& other);
    GraphicsSystem& operator=(const GraphicsSystem& other);

    void Initialize();
    void Update(float deltaTime);
    void Render(float deltaTime);
    void Cleanup();

    Shader* GetShader() const;

    struct GLObject {
        glm::vec2 orientation;
        glm::vec2 scaling;
        glm::vec2 position;
        glm::vec3 color;
        glm::mat3 mdl_xform;
        glm::mat3 mdl_to_ndc_xform;

        void init (glm::vec2 orientation, glm::vec2 scaling, glm::vec2 position);
        void update(GLdouble time_per_frame);
        void draw(Shader* shader, const GraphicsSystem& vao) const;
    };
private:
    GLuint m_VAO;
    GLuint m_VBO;       // VBO for vertex positions
    GLuint m_UVBO;      // Separate VBO for texture coordinates (UVs)
    GLuint m_EBO;       // Element Buffer Object
    GLuint m_Texture;
    std::unique_ptr<Shader> m_Shader;
    std::unique_ptr<AnimationData> m_AnimationData;  // Pointer to the AnimationData instance

    void ReleaseResources();
};
