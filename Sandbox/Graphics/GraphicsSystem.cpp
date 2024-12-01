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
#include "GUIGameViewport.h"


#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__,__LINE__))
// Function to clear OpenGL errors
static void GLClearError() {
    while (glGetError() != GL_NO_ERROR);
}

// Function to log OpenGL errors
static bool GLLogCall(const char* function, const char* file, int line) {
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] (" << error << "): " << function << " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}

// Function to load a texture from a file
GraphicsSystem::GraphicsSystem()
    : m_VAO(0), m_VBO(0), m_UVBO(0), m_EBO(0), m_Texture(0) {
    // Initialize AnimationData with total frames, frame duration, columns, rows of the spritesheet
    m_AnimationData = std::make_unique<AnimationData>(48, 0.03f, 4, 6);
    vps = new std::vector<GLViewport>();
    vps->push_back({ 0, 0, GLFWFunctions::windowWidth, GLFWFunctions::windowHeight });
    glViewport((*vps)[0].x, (*vps)[0].y, (*vps)[0].width, (*vps)[0].height);

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

    int width = assetsManager.texWidthGet();
    int height = assetsManager.texHeightGet();

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

    assetsManager.GetShader("shader2")->Bind();

    int location = assetsManager.GetShader("shader2")->GetUniformLocation("u_Color");
    ASSERT(location != -1);
    GLCall(glUniform4f(location, 0.8f, 0.3f, 0.8f, 1.0f));

    assetsManager.GetShader("shader2")->Unbind();
}

void GraphicsSystem::update() {}

void GraphicsSystem::Update(float deltaTime, GLboolean isAnimated) {
    if (isAnimated == GL_TRUE) {
        if (!GameViewWindow::getPaused()) {
            m_AnimationData->Update(deltaTime);
        }
        //m_AnimationData->Update(deltaTime);
        const auto& uvCoords = m_AnimationData->GetCurrentUVs();

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

    GLint w{ GLFWFunctions::windowWidth }, h{ GLFWFunctions::windowHeight };
    static GLint old_w{}, old_h{};
    // update viewport settings in vps only if window's dimension change
    if (w != old_w || h != old_h)
    {
        (*vps)[0] = { 0, 0, w , h };
        old_w = w;
        old_h = h;
    }
	glViewport((*vps)[0].x, (*vps)[0].y, (*vps)[0].width, (*vps)[0].height);
}

void GraphicsSystem::Render(float deltaTime) {
    (void)deltaTime;
    glClear(GL_COLOR_BUFFER_BIT);
}

void GraphicsSystem::cleanup() {
    ReleaseResources();
    delete m_AnimationData.release();
    delete vps;
    vps = nullptr;
}

void GraphicsSystem::ReleaseResources() {
    glDeleteBuffers(1, &m_EBO);
    glDeleteBuffers(1, &m_UVBO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteTextures(1, &m_Texture);
    glDeleteVertexArrays(1, &m_VAO);
}

myMath::Matrix3x3 GraphicsSystem::UpdateObject(myMath::Vector2D objPos, myMath::Vector2D objScale, myMath::Vector2D objOri, myMath::Matrix3x3 viewMatrix) {
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

    glm::mat3 viewMat = myMath::Matrix3x3::ConvertToGLMMat3(viewMatrix);

    mdl_xform = Translating * (Rotating * Scaling);
    mdl_to_ndc_xform = projMat * viewMat * mdl_xform;
    myMath::Matrix3x3 final_xform = myMath::Matrix3x3::ConvertToMatrix3x3(mdl_to_ndc_xform);
    return final_xform;
}

void GraphicsSystem::drawDebugOBB(TransformComponent transform, myMath::Matrix3x3 viewMatrix) {

    glBegin(GL_LINES);

    // Set color for debug lines
    glColor3f(1.0f, 0.0f, 0.0f); // Red color for debug lines

    float left, right, top, bottom;
    left = -transform.scale.GetX() / 2.0f;
    right = transform.scale.GetX() / 2.0f;
    top = transform.scale.GetY() / 2.0f;
    bottom = -transform.scale.GetY() / 2.0f;

    // Define the corners of the AABB in homogeneous coordinates
    glm::vec4 bottomLeft(left, bottom, 0.0f, 1.0f);
    glm::vec4 bottomRight(right, bottom, 0.0f, 1.0f);
    glm::vec4 topRight(right, top, 0.0f, 1.0f);
    glm::vec4 topLeft(left, top, 0.0f, 1.0f);

    glm::mat3 viewMat = myMath::Matrix3x3::ConvertToGLMMat3(viewMatrix);

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
    // Define the rotation angle (in radians)
    float angle = glm::radians(transform.orientation.GetX()); // Example: 45 degrees

    // Create the rotation matrix
    glm::mat4 rotationMat = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 0.0f, 1.0f));

    // Define the translation vector using transform.position
    glm::vec3 translationVec(transform.position.GetX(), transform.position.GetY(), 0.0f);

    // Create the translation matrix
    glm::mat4 translationMat = glm::translate(glm::mat4(1.0f), translationVec);

    // Apply the projection, view, rotation, and translation matrices to each corner
    bottomLeft = projMat * viewMat4 * translationMat * rotationMat * bottomLeft;
    bottomRight = projMat * viewMat4 * translationMat * rotationMat * bottomRight;
    topRight = projMat * viewMat4 * translationMat * rotationMat * topRight;
    topLeft = projMat * viewMat4 * translationMat * rotationMat * topLeft;

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

void GraphicsSystem::drawDebugCircle(TransformComponent transform, myMath::Matrix3x3 viewMatrix) {

    glBegin(GL_LINE_LOOP); // Use GL_LINE_LOOP to connect the points in a circular loop

    // Set color for debug circle
    glColor3f(1.0f, 0.0f, 0.0f); // Green color for debug circle

    // Define the radius of the circle (assuming scale's X represents the diameter)
    float radius = transform.scale.GetX() / 2.0f;

    // Define the number of segments for the circle approximation
    int numSegments = 100; // Higher value gives smoother circle

    // Define the rotation angle (in radians)
    float angle = glm::radians(transform.orientation.GetX());

    // Create the rotation matrix
    glm::mat4 rotationMat = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 0.0f, 1.0f));

    // Define the translation vector using transform.position
    glm::vec3 translationVec(transform.position.GetX(), transform.position.GetY(), 0.0f);

    // Create the translation matrix
    glm::mat4 translationMat = glm::translate(glm::mat4(1.0f), translationVec);

    // Convert the view matrix
    glm::mat3 viewMat = myMath::Matrix3x3::ConvertToGLMMat3(viewMatrix);
    glm::mat4 viewMat4 = {
        viewMat[0][0], viewMat[0][1], viewMat[0][2], 0,
        viewMat[1][0], viewMat[1][1], viewMat[1][2], 0,
        0,             0,             viewMat[2][2], 0,
        viewMat[2][0], viewMat[2][1], 1,             1
    };

    // Create the projection matrix
    glm::mat4 projMat = glm::ortho(
        -GLFWFunctions::windowWidth / 2.0f,  // left
        GLFWFunctions::windowWidth / 2.0f,   // right
        -GLFWFunctions::windowHeight / 2.0f, // bottom
        GLFWFunctions::windowHeight / 2.0f   // top
    );

    // Iterate over each segment to compute the vertices of the circle
    for (int i = 0; i < numSegments; ++i) {
        float theta = 2.0f * glm::pi<float>() * float(i) / float(numSegments); // Current angle

        // Calculate the position of each point on the circle in local coordinates
        glm::vec4 point(radius * cos(theta), radius * sin(theta), 0.0f, 1.0f);

        // Apply transformation matrices
        point = projMat * viewMat4 * translationMat * rotationMat * point;

        // Render the vertex
        glVertex2f(point.x, point.y);
    }

    glEnd();
}


void GraphicsSystem::DrawObject(DrawMode mode, const GLuint texture, myMath::Matrix3x3 xform) {
    // load shader program in use by this object
    if (mode == DrawMode::TEXTURE)
        assetsManager.GetShader("shader1")->Bind();
    else
        assetsManager.GetShader("shader2")->Bind();
    glBindVertexArray(m_VAO);
    glBindTexture(GL_TEXTURE_2D, texture);
    glm::mat3 mdl_xform(1.0f);
    mdl_xform = myMath::Matrix3x3::ConvertToGLMMat3(xform);

    GLint uniformLoc = assetsManager.GetShader("shader2")->GetUniformLocation("uModel_to_NDC");
    if (uniformLoc != -1) {
        glUniformMatrix3fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(mdl_xform));
    }

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

    // unbind VAO
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    assetsManager.GetShader("shader2")->Unbind();
}


