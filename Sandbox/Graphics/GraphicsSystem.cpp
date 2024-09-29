#include "GraphicsSystem.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>


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

    // Load texture 1
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

    // Loat texture 2
    glGenTextures(1, &m_Texture2);
    glBindTexture(GL_TEXTURE_2D, m_Texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    data = stbi_load("./Graphics/Assets/image.png", &width, &height, &nrChannels, 0);
    if (data) {
		GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);
    }
    else {
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
        quadWidth / 2.0f,  quadHeight / 2.0f, 1.0f,  // top right
        quadWidth / 2.0f, -quadHeight / 2.0f, 1.0f,  // bottom right
       -quadWidth / 2.0f, -quadHeight / 2.0f, 1.0f,  // bottom left
       -quadWidth / 2.0f,  quadHeight / 2.0f, 1.0f   // top left
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
    /*m_Shader->Bind();
    glBindTexture(GL_TEXTURE_2D, m_Texture);
    glBindVertexArray(m_VAO);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindTexture(GL_TEXTURE_2D, 0);*/
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(100, 100, 1400, 700);
}

void GraphicsSystem::Cleanup() {
    ReleaseResources();
}

Shader* GraphicsSystem::GetShader() const{
	return m_Shader.get();
}

void GraphicsSystem::ReleaseResources() {
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_UVBO);
    glDeleteBuffers(1, &m_EBO);
    glDeleteTextures(1, &m_Texture);
    glDeleteVertexArrays(1, &m_VAO);
}

void GraphicsSystem::GLObject::init(glm::vec2 rhsOrientation,glm::vec2 rhsScaling,glm::vec2 rhsPosition) {
	orientation = rhsOrientation;
    scaling = rhsScaling;
    position = rhsPosition;
	mdl_xform = glm::mat3{ 1.0 };
	mdl_to_ndc_xform = glm::mat3{ 1.0 };
}

void GraphicsSystem::GLObject::update(GLdouble time_per_frame) {
    glm::mat3 Scaling{ 1.0 }, Rotating{ 1.0 }, Translating{ 1.0 }, NDC{ 0 };
    GLfloat aspect_ratio = 1600 / 900;// TODO::change this to be calculated based on the window size

    Scaling =
    {
        scaling.x, 0		 , 0,
        0        , scaling.y , 0,
        0        , 0         , 1
    };

    orientation.x += static_cast<GLfloat>(orientation.y * time_per_frame * 100.0f);

    Rotating =
    {
         glm::cos(glm::radians(orientation.x)), glm::sin(glm::radians(orientation.x)), 0,
        -glm::sin(glm::radians(orientation.x)), glm::cos(glm::radians(orientation.x)), 0,
         0, 0, 1
    };
    Translating =
    {
        1,		    0 ,		     0,
        0,		    1 ,		     0,
        position.x, position.y , 1
    };
    // TODO:: change the NDC matrix to be calculated based on the window size
    NDC = 
    {
        2.0f / 1600.0f,  0,              0,
        0,               2.0f / 900.0f,  0,
        0,               0,              1 };

    mdl_xform = Translating * (Rotating * Scaling);
    mdl_to_ndc_xform =  mdl_xform;
}

void GraphicsSystem::GLObject::draw(Shader* shader, const GLuint vao, const GLuint tex) const{
    // load shader program in use by this object
    shader->Bind();
    // bind VAO of this object
    glBindVertexArray(vao);
    glBindTexture(GL_TEXTURE_2D, tex);

    GLint uniformLoc = shader->GetUniformLocation("uModel_to_NDC");
    if (uniformLoc != -1) {
		glUniformMatrix3fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(mdl_to_ndc_xform));
	}

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

    // unbind VAO
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
	// unbind shader program
	shader->Unbind();
}
