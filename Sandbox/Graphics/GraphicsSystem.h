#pragma once

#include <GL/glew.h>
#include <memory>
#include "Shader.h"
#include "AnimationData.h"

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
