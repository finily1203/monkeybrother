/*
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Liu YaoTing (yaoting.liu), Javier Chua (javierjunliang.chua)
@team   :  MonkeHood
@course :  CSD2401
@file   :  GraphicsSystem.cpp
@brief  :  This file contains the implementation of the GraphicsSystem class,
           the GraphicsSystem class is responsible for rendering the game objects.

* Liu YaoTing (yaoting.liu) :
        - Implementated OpenGL debug function GL Call: It is used to check for OpenGL errors and log them.
        - Loaded textures and shaders into the GraphicsSystem.
        - Implemented the initialise function such as creating the VAO, VBO, EBO, UVBO, and loading the textures.
        - Implemented the initialization of uvCoords.
        - Implemented GLObject init(Data member Initialization), update (Model Transformations),
          draw functions.

* Javier Chua (javierjunliang.chua) :
        - Implemented the initialise function such as creating the VAO, VBO, EBO, UVBO, and loading the textures.
        - Implemented the update function to update the UV coordinates according to animation.
        - Loaded textures and shaders into the GraphicsSystem.
        - Implemented function to change sprite animation according to the action.
        - Implemented drawDebugLines function to draw the bounding box of the sprite,
          and the update function to update the model transformation matrix.
        
 File Contributions: Liu YaoTing (50%), Javier Chua (50%)

/*_______________________________________________________________________________________________________________*/

#include "GraphicsSystem.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include "GlobalCoordinator.h"

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
    cleanup();
}

//For perfomance viewer
SystemType GraphicsSystem::getSystem() {
    return GraphicsSystemType;
}

void GraphicsSystem::initialise() {


    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //ShaderProgramSource source = Shader::ParseShader("./Graphics/Basic.shader");
    //m_Shader = std::make_unique<Shader>(source.VertexSource, source.FragmentSource);
    //if (!m_Shader->IsCompiled()) {
    //    std::cerr << "Shader compilation failed." << std::endl;
    //    return;
    //}

    //ShaderProgramSource source2 = Shader::ParseShader("./Graphics/Basic1.shader");
    //m_Shader2 = std::make_unique<Shader>(source2.VertexSource, source2.FragmentSource);
    //if (!m_Shader2->IsCompiled()) {
    //    std::cerr << "Shader compilation failed." << std::endl;
    //    return;
    //}

    loadShaderAssets();
    loadTextureAssets();
    std::cout << assetsManager.texWidthGet() << std::endl;
    std::cout << assetsManager.texHeightGet() << std::endl;
    std::cout << assetsManager.nrChannelsGet() << std::endl;

    int width = assetsManager.texWidthGet();
    int height = assetsManager.texHeightGet();

    //// Load texture 1
    //int width, height, nrChannels;
    //stbi_set_flip_vertically_on_load(true);
    //unsigned char* data = stbi_load("./Graphics/Assets/player.png", &width, &height, &nrChannels, 0);

    //if (data) {
    //    GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;

    //    // Load texture into OpenGL
    //    glGenTextures(1, &m_Texture);
    //    glBindTexture(GL_TEXTURE_2D, m_Texture);
    //    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    //    glGenerateMipmap(GL_TEXTURE_2D);

    //    // Set texture parameters to prevent bleeding
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // Use nearest filtering
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // Use nearest filtering

    //    stbi_image_free(data);
    //}
    //else {
    //    std::cerr << "Failed to load texture!" << std::endl;
    //    stbi_image_free(data);
    //    return;
    //}

    //// Loat texture 2
    //glGenTextures(1, &m_Texture2);
    //glBindTexture(GL_TEXTURE_2D, m_Texture2);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //data = stbi_load("./Graphics/Assets/image.png", &width, &height, &nrChannels, 0);
    //if (data) {
    //    GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
    //    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    //    glGenerateMipmap(GL_TEXTURE_2D);
    //    stbi_image_free(data);
    //}
    //else {
    //    std::cerr << "Failed to load texture!" << std::endl;
    //    stbi_image_free(data);
    //    return;
    //}

    //// Load texture 3
    //glGenTextures(1, &m_Texture3);
    //glBindTexture(GL_TEXTURE_2D, m_Texture3);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //data = stbi_load("./Graphics/Assets/image1.jpg", &width, &height, &nrChannels, 0);
    //if (data) {
    //    GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
    //    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    //    glGenerateMipmap(GL_TEXTURE_2D);
    //    stbi_image_free(data);
    //}
    //else {
    //    std::cerr << "Failed to load texture!" << std::endl;
    //    stbi_image_free(data);
    //    return;
    //}



    // Calculate the frame width and height based on the number of columns
    int columns = 1;  // Adjust to match your spritesheet
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
    //m_Shader2->Bind();
    assetsManager.GetShader("shader2")->Bind();

    //int location = m_Shader2->GetUniformLocation("u_Color");
    int location = assetsManager.GetShader("shader2")->GetUniformLocation("u_Color");
    ASSERT(location != -1);
    GLCall(glUniform4f(location, 0.8f, 0.3f, 0.8f, 1.0f));

    //m_Shader2->Unbind();
    assetsManager.GetShader("shader2")->Unbind();
}

void GraphicsSystem::update() {}

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

void GraphicsSystem::cleanup() {
    ReleaseResources();
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
    color = glm::vec3{ 1.0, 1.0, 1.0 };
    GLboolean isAnimated = GL_FALSE;
}

void GraphicsSystem::GLObject::update(GLdouble time_per_frame) {
    glm::mat3 Scaling{ 1.0 }, Rotating{ 1.0 }, Translating{ 1.0 }, NDC{ 0 };
    GLfloat aspect_ratio = 1600.f / 900.f;// TODO::change this to be calculated based on the window size

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
    mdl_to_ndc_xform = NDC * mdl_xform;
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



glm::mat3x3 GraphicsSystem::UpdateObject(GLdouble deltaTime, glm::vec2 objPos, glm::vec2 objScale, glm::vec2 objOri, glm::mat3 viewMat) {
    glm::mat3 Scaling{ 1.0 }, Rotating{ 1.0 }, Translating{ 1.0 }, projMat{ 1.0 }, mdl_xform{ 1.0 }, mdl_to_ndc_xform{ 0 };

    Translating =
    {
        1,      0 ,       0,
        0,      1 ,       0,
        objPos.GetX(),   objPos.GetY() , 1
    };
    Scaling =
    {
        objScale.GetX()  ,0          ,0,
        0           ,objScale.GetY()    ,0,
        0           ,0             ,1
    };


    objOri.SetX(objOri.GetX() + objOri.GetY());

    Rotating =
    {
         glm::cos(glm::radians(objOri.GetX())), glm::sin(glm::radians(objOri.GetX())), 0,
        -glm::sin(glm::radians(objOri.GetX())), glm::cos(glm::radians(objOri.GetX())), 0,
         0, 0, 1
    };

    projMat = glm::ortho(
        -GLFWFunctions::windowWidth / 2.0f,  // left
        GLFWFunctions::windowWidth / 2.0f,   // right
        -GLFWFunctions::windowHeight / 2.0f, // bottom
        GLFWFunctions::windowHeight / 2.0f   // top
    );
    mdl_xform = Translating * (Rotating * Scaling);
    mdl_to_ndc_xform = projMat * viewMat * mdl_xform;
    return mdl_to_ndc_xform;
}

void GraphicsSystem::drawDebugAABB(AABBComponent aabb, glm::mat3 viewMat) {

    glBegin(GL_LINES);

    // Set color for debug lines
    glColor3f(1.0f, 0.0f, 0.0f); // Red color for debug lines

    // Define the corners of the AABB in homogeneous coordinates
    glm::vec4 bottomLeft(aabb.left, aabb.bottom, 0.0f, 1.0f);
    glm::vec4 bottomRight(aabb.right, aabb.bottom, 0.0f, 1.0f);
    glm::vec4 topRight(aabb.right, aabb.top, 0.0f, 1.0f);
    glm::vec4 topLeft(aabb.left, aabb.top, 0.0f, 1.0f);

    // Create the projection matrix
    glm::mat4 projMat = glm::ortho(
        -GLFWFunctions::windowWidth / 2.0f,  // left
        GLFWFunctions::windowWidth / 2.0f,   // right
        -GLFWFunctions::windowHeight / 2.0f, // bottom
        GLFWFunctions::windowHeight / 2.0f   // top
    );
    glm::mat4 viewMat4 = {
		viewMat[0][0], viewMat[0][1], viewMat[0][2], 0,
		viewMat[1][0], viewMat[1][1], viewMat[1][2], 0,
		0,             0,             viewMat[2][2], 0,
		viewMat[2][0], viewMat[2][1], 1,             1
    };

    // Apply the projection matrix to each corner
    bottomLeft = projMat * viewMat4 *bottomLeft;
	bottomRight = projMat * viewMat4 * bottomRight;
	topRight = projMat * viewMat4 * topRight;
	topLeft = projMat * viewMat4 * topLeft;


    // Draw the AABB lines using the transformed coordinates
    glVertex2f(bottomLeft.x, bottomLeft.y);   // Bottom left
    glVertex2f(bottomRight.x, bottomRight.y); // Bottom right

    glVertex2f(bottomRight.x, bottomRight.y); // Bottom right
    glVertex2f(topRight.x, topRight.y);       // Top right

    glVertex2f(topRight.x, topRight.y);       // Top right
    glVertex2f(topLeft.x, topLeft.y);         // Top left

    glVertex2f(topLeft.x, topLeft.y);         // Top left
    glVertex2f(bottomLeft.x, bottomLeft.y);   // Back to bottom left

    glEnd();
}

void GraphicsSystem::DrawObject(DrawMode mode, const GLuint texture, glm::mat3 xform) {
    // load shader program in use by this object
    if (mode == DrawMode::TEXTURE)
        //m_Shader->Bind();
        assetsManager.GetShader("shader1")->Bind();
    else
        //m_Shader2->Bind();
        assetsManager.GetShader("shader2")->Bind();
    // bind VAO of this object
    glBindVertexArray(m_VAO);
    glBindTexture(GL_TEXTURE_2D, texture);

    //GLint uniformLoc = m_Shader2->GetUniformLocation("uModel_to_NDC");
    GLint uniformLoc = assetsManager.GetShader("shader2")->GetUniformLocation("uModel_to_NDC");
    if (uniformLoc != -1) {
        glUniformMatrix3fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(xform));
    }

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

    // unbind VAO
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    // unbind shader program
    //m_Shader2->Unbind();
    assetsManager.GetShader("shader2")->Unbind();
}

void GraphicsSystem::loadShaderAssets() const {
    std::string jsonFilePath = FilePathManager::GetAssetsJSONPath();
    std::ifstream file(jsonFilePath);
    nlohmann::json jsonObj;

    if (file.is_open())
    {
        file >> jsonObj;
        file.close();
    }

    for (const auto& shaderAsset : jsonObj["shaderAssets"])
    {
        std::string shaderName = shaderAsset["id"].get<std::string>();
        std::string relativePath = shaderAsset["filePath"].get<std::string>();

        std::string shaderFilePath = FilePathManager::GetExecutablePath() + "\\..\\..\\..\\" + relativePath;
        assetsManager.LoadShader(shaderName, shaderFilePath);
    }
}

void GraphicsSystem::loadTextureAssets() const {
    std::string jsonFilePath = FilePathManager::GetAssetsJSONPath();
    std::ifstream file(jsonFilePath);
    nlohmann::json jsonObj;

    if (file.is_open())
    {
        file >> jsonObj;
        file.close();
    }

    for (const auto& textureAsset : jsonObj["textureAssets"])
    {
        std::string textureName = textureAsset["id"].get<std::string>();
        std::string relativePath = textureAsset["filePath"].get<std::string>();

        std::string textureFilePath = FilePathManager::GetExecutablePath() + "\\..\\..\\..\\" + relativePath;
        assetsManager.LoadTexture(textureName, textureFilePath);
    }
}