#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "GraphicsSystem.h"
#include <iostream>

GraphicsSystem::GraphicsSystem()
    : m_VAO(0), m_VBO(0), m_Texture(0) {
}

GraphicsSystem::~GraphicsSystem() {
    Cleanup();
}

GraphicsSystem::GraphicsSystem(const GraphicsSystem& other)
    : m_VAO(0), m_VBO(0), m_Texture(0) {
    if (other.m_Shader) {
        m_Shader = std::make_unique<Shader>(*other.m_Shader);
    }
}

GraphicsSystem& GraphicsSystem::operator=(const GraphicsSystem& other) {
    if (this != &other) {
        Cleanup();

        if (other.m_Shader) {
            m_Shader = std::make_unique<Shader>(*other.m_Shader);
        }
    }
    return *this;
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
    
    float vertices[] = {
        // positions          // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 
         0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 
        -0.5f,  0.5f, 0.0f,   0.0f, 1.0f  
    };

    unsigned int indices[] = {
        0, 1, 3,   
        1, 2, 3   
    };

    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0); 
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))); // TexCoords
    glEnableVertexAttribArray(1);

    GLuint EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // disable vertical flip on load

    unsigned char* data = stbi_load("./Graphics/Assets/moss.png", &width, &height, &nrChannels, 0);

    if (data) {
        GLenum format;
        if (nrChannels == 4) {
            format = GL_RGBA;  
        }
        else if (nrChannels == 3) {
            format = GL_RGB;
        }

        glGenTextures(1, &m_Texture);
        glBindTexture(GL_TEXTURE_2D, m_Texture);

       
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else {
        std::cerr << "Failed to load texture!" << std::endl;
        stbi_image_free(data);
    }
    glBindTexture(GL_TEXTURE_2D, 0); 
}

void GraphicsSystem::Update() {
    //empty for now
}

void GraphicsSystem::Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_Shader->Bind();
    glBindTexture(GL_TEXTURE_2D, m_Texture);

    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);  

    glBindVertexArray(0);

    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << err << std::endl;
    }
}



void GraphicsSystem::Cleanup() {
    ReleaseResources();
}

void GraphicsSystem::ReleaseResources() {
    if (m_VBO) {
        glDeleteBuffers(1, &m_VBO);
        m_VBO = 0;
    }
    if (m_VAO) {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }
    if (m_Texture) {
        glDeleteTextures(1, &m_Texture);
        m_Texture = 0;
    }
    if (m_Shader) {
        m_Shader.reset();
    }
}
