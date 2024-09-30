#include "GraphicsSystem.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__,__LINE__))

static void GLClearError() {
    while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line) {
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] (" << error << "): " << function << " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}

GraphicsSystem::GraphicsSystem()
    : m_VAO(0), m_VBO(0), m_UVBO(0), m_EBO(0), m_Texture(0) {
    // Initialize AnimationData with total frames, frame duration, columns, rows of the spritesheet
    m_AnimationData = std::make_unique<AnimationData>(16, 0.2f, 4, 4);
    idleAnimation = std::make_unique<AnimationData>(16, 0.2f, 4, 4);

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

    ShaderProgramSource source2 = Shader::ParseShader("./Graphics/Basic1.shader");
    m_Shader2 = std::make_unique<Shader>(source2.VertexSource, source2.FragmentSource);
    if (!m_Shader2->IsCompiled()) {
		std::cerr << "Shader compilation failed." << std::endl;
		return;
	}

    // Load texture 1
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("./Graphics/Assets/player.png", &width, &height, &nrChannels, 0);

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
    }
    else {
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

    // Load texture 3
    glGenTextures(1, &m_Texture3);
    glBindTexture(GL_TEXTURE_2D, m_Texture3);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    data = stbi_load("./Graphics/Assets/image1.jpg", &width, &height, &nrChannels, 0);
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

    float uvCoord[] = {
        1.0f, 1.0f,  // top right
        1.0f, 0.0f,  // bottom right
        0.0f, 0.0f,  // bottom left
        0.0f, 1.0f   // top left
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 4, uvCoord, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    // EBO
    glGenBuffers(1, &m_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // get the location of the uniform variable in the shader
    /*int location = m_Shader2->GetUniformLocation("u_Color");
    ASSERT(location != -1);
    GLCall(glUniform4f(location, 0.8f, 0.3f, 0.8f, 1.0f));*/
}

void GraphicsSystem::Update(float deltaTime, GLboolean isAnimated) {
    if (isAnimated == GL_TRUE) {
        m_AnimationData->Update(deltaTime);
        const auto& uvCoords = m_AnimationData->GetCurrentUVs();
        idleAnimation->Update(deltaTime);
        const auto& idleUVs = idleAnimation->GetCurrentUVs();
        glBindBuffer(GL_ARRAY_BUFFER, m_UVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec2) * 4, uvCoords.data());
    }
    else {
        // Update the UV coordinates for the current frame
        float uvCoord[] = {
            1.0f, 1.0f,  // top right
            1.0f, 0.0f,  // bottom right
            0.0f, 0.0f,  // bottom left
            0.0f, 1.0f   // top left
        };
        glBindBuffer(GL_ARRAY_BUFFER, m_UVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec2) * 4, uvCoord);
    }
}
void GraphicsSystem::SetCurrentAction(int actionRow) {
    if (m_AnimationData) {
        m_AnimationData->SetCurrentAction(actionRow);  // Switch the action (e.g., walk, attack)
    }
}
void GraphicsSystem::Render(float deltaTime) {
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, 1600, 900);
}

void GraphicsSystem::Cleanup() {
    ReleaseResources();
}

Shader* GraphicsSystem::GetShader() const {
    return m_Shader.get();
}

void GraphicsSystem::ReleaseResources() {
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_UVBO);
    glDeleteBuffers(1, &m_EBO);
    glDeleteTextures(1, &m_Texture);
    glDeleteVertexArrays(1, &m_VAO);
}

void GraphicsSystem::GLObject::init(glm::vec2 rhsOrientation, glm::vec2 rhsScaling, glm::vec2 rhsPosition) {
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
    mdl_to_ndc_xform = mdl_xform;
}

void GraphicsSystem::GLObject::draw(Shader* shader, const GLuint vao, const GLuint tex) const {
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

void GraphicsSystem::drawDebugLines(const GLObject& obj) {

    glBegin(GL_LINES);

    // Set color for debug lines
    glColor3f(1.0f, 0.0f, 0.0f); // Red color for debug lines


    float spriteWidth = obj.scaling.x / 4; // Width of one sprite (assuming 4 columns in the sprite sheet)
    float spriteHeight = obj.scaling.y;     // Height of the sprite


    float halfWidth = spriteWidth * 0.5f;
    float halfHeight = spriteHeight * 0.5f;


    glm::vec2 bottomLeft = glm::vec2(-halfWidth, -halfHeight);
    glm::vec2 bottomRight = glm::vec2(halfWidth, -halfHeight);
    glm::vec2 topRight = glm::vec2(halfWidth, halfHeight);
    glm::vec2 topLeft = glm::vec2(-halfWidth, halfHeight);

    // Create a rotation matrix based on the object's rotation angle (around the z-axis)
    glm::mat2 Rotating =
    {
         glm::cos(glm::radians(obj.orientation.x)), glm::sin(glm::radians(obj.orientation.x)),
        -glm::sin(glm::radians(obj.orientation.x)), glm::cos(glm::radians(obj.orientation.x))
    };

    // Rotate and translate each corner based on the object's rotation and position
    glm::vec2 rotatedBottomLeft = obj.position + Rotating * bottomLeft;
    glm::vec2 rotatedBottomRight = obj.position + Rotating * bottomRight;
    glm::vec2 rotatedTopRight = obj.position + Rotating * topRight;
    glm::vec2 rotatedTopLeft = obj.position + Rotating * topLeft;

    // Draw the AABB lines
    glVertex2f(rotatedBottomLeft.x, rotatedBottomLeft.y); // Bottom left
    glVertex2f(rotatedBottomRight.x, rotatedBottomRight.y); // Bottom right

    glVertex2f(rotatedBottomRight.x, rotatedBottomRight.y); // Bottom right
    glVertex2f(rotatedTopRight.x, rotatedTopRight.y); // Top right

    glVertex2f(rotatedTopRight.x, rotatedTopRight.y); // Top right
    glVertex2f(rotatedTopLeft.x, rotatedTopLeft.y); // Top left

    glVertex2f(rotatedTopLeft.x, rotatedTopLeft.y); // Top left
    glVertex2f(rotatedBottomLeft.x, rotatedBottomLeft.y); // Back to bottom left

    glEnd();
}
