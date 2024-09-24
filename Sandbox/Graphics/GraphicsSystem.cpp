#include "GraphicsSystem.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include <glm/glm.hpp>

GraphicsSystem::GraphicsSystem()
    : m_VAO(0), m_VBO(0), m_UVBO(0), m_EBO(0), m_Texture(0) {
    // Initialize AnimationData with total frames, frame duration, columns, rows of the spritesheet
    m_AnimationData = std::make_unique<AnimationData>(4, 0.1f, 4, 1); // 5 frames
}

GraphicsSystem::~GraphicsSystem() {
    Cleanup();
}

void GraphicsSystem::Initialize() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    ShaderProgramSource source = Shader::ParseShader("./Graphics/Basic.shader");
    m_Shader = std::make_unique<Shader>(source.VertexSource, source.FragmentSource);
    if (!m_Shader->IsCompiled()) {
        std::cerr << "Shader compilation failed." << std::endl;
        return;
    }

    // Load texture
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("./Graphics/Assets/demon_spritesheet.png", &width, &height, &nrChannels, 0);
    
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;

        // Load texture into OpenGL
        glGenTextures(1, &m_Texture);
        glBindTexture(GL_TEXTURE_2D, m_Texture);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Set texture parameters to prevent bleeding
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // Use nearest filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // Use nearest filtering

        stbi_image_free(data);
    } else {
        std::cerr << "Failed to load texture!" << std::endl;
        stbi_image_free(data);
        return;
    }

    // Calculate the frame width and height based on the number of columns
    int columns = 4;  // Adjust to match your spritesheet
    int rows = 1;     // Adjust to match your spritesheet
    float frameWidth = static_cast<float>(width) / columns;
    float frameHeight = static_cast<float>(height) / rows;

    // Adjust the quad size based on a single frame's dimensions
    float quadWidth = frameWidth / width;  // Normalize to texture size (0.0 to 1.0)
    float quadHeight = frameHeight / height;

    // Quad vertices using normalized frame size (0.0 to 1.0)
    float vertices[] = {
        // positions                    // Assume center of the quad is at (0, 0)
        quadWidth / 2.0f,  quadHeight / 2.0f, 0.0f,  // top right
        quadWidth / 2.0f, -quadHeight / 2.0f, 0.0f,  // bottom right
       -quadWidth / 2.0f, -quadHeight / 2.0f, 0.0f,  // bottom left
       -quadWidth / 2.0f,  quadHeight / 2.0f, 0.0f   // top left
    };

    unsigned int indices[] = {
        0, 1, 3,  // First triangle
        1, 2, 3   // Second triangle
    };

    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    // Position VBO
    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // UV VBO (initial UVs)
    glGenBuffers(1, &m_UVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_UVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 4, nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    // EBO
    glGenBuffers(1, &m_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void GraphicsSystem::Update(float deltaTime) {
    m_AnimationData->Update(deltaTime);

    const auto& uvCoords = m_AnimationData->GetCurrentUVs();
    glBindBuffer(GL_ARRAY_BUFFER, m_UVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec2) * 4, uvCoords.data());
}

void GraphicsSystem::Render(float deltaTime) {
    m_Shader->Bind();
    glBindTexture(GL_TEXTURE_2D, m_Texture);
    glBindVertexArray(m_VAO);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void GraphicsSystem::Cleanup() {
    ReleaseResources();
}

void GraphicsSystem::ReleaseResources() {
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_UVBO);
    glDeleteBuffers(1, &m_EBO);
    glDeleteTextures(1, &m_Texture);
    glDeleteVertexArrays(1, &m_VAO);
}
