#include "GUIGridSystem.h"
#include "GlobalCoordinator.h"
#include "GUIConsole.h"

GridSystem::GridSystem()
    : m_cellSize(64.0f), m_gridWidth(50.0f), m_gridHeight(50.0f), m_isInitialized(false)
{
}

GridSystem::~GridSystem()
{
}

void GridSystem::initialise(float cellSize, float gridWidth, float gridHeight)
{
    m_cellSize = cellSize;
    m_gridWidth = gridWidth;
    m_gridHeight = gridHeight;
    m_isInitialized = true;
}

myMath::Vector2D GridSystem::snapToGrid(myMath::Vector2D position) {
    if (!m_isInitialized) {
        return position;
    }

    // Calculate the cell indices
    float cellX = std::floor(position.GetX() / m_cellSize) + 0.5f;
    float cellY = std::floor(position.GetY() / m_cellSize) + 0.5f;

    // Calculate the center of the cell
    float centerX = cellX * m_cellSize;
    float centerY = cellY * m_cellSize;

    return myMath::Vector2D(centerX, centerY);
}

void GridSystem::drawGrid() {
    if (!m_isInitialized) {
        return; // Don't draw if not initialized
    }

    // Get camera settings for proper grid display
    myMath::Vector2D cameraPos = cameraSystem.getCameraPosition();
    float zoom = cameraSystem.getCameraZoom();

    // Calculate visible area of the grid
    float halfScreenWidth = GLFWFunctions::windowWidth / (2.0f * zoom);
    float halfScreenHeight = GLFWFunctions::windowHeight / (2.0f * zoom);

    // Calculate grid boundaries
    float gridLeft = cameraPos.GetX() - halfScreenWidth;
    float gridRight = cameraPos.GetX() + halfScreenWidth;
    float gridBottom = cameraPos.GetY() - halfScreenHeight;
    float gridTop = cameraPos.GetY() + halfScreenHeight;

    // Adjust grid start positions to align with grid cells
    float startX = std::floor(gridLeft / m_cellSize) * m_cellSize;
    float startY = std::floor(gridBottom / m_cellSize) * m_cellSize;

    // Set up projection matrix (same as used for other rendering)
    glm::mat4 projMat = glm::ortho(
        -GLFWFunctions::windowWidth / 2.0f,
        GLFWFunctions::windowWidth / 2.0f,
        -GLFWFunctions::windowHeight / 2.0f,
        GLFWFunctions::windowHeight / 2.0f
    );

    // Apply camera view matrix
    glm::mat3 viewMat = myMath::Matrix3x3::ConvertToGLMMat3(cameraSystem.getViewMatrix());
    glm::mat4 viewMat4 = {
        viewMat[0][0], viewMat[0][1], viewMat[0][2], 0,
        viewMat[1][0], viewMat[1][1], viewMat[1][2], 0,
        0,             0,             viewMat[2][2], 0,
        viewMat[2][0], viewMat[2][1], 1,             1
    };

    // Save OpenGL state
    glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT | GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Temporarily disable texturing
    GLboolean textureEnabled;
    glGetBooleanv(GL_TEXTURE_2D, &textureEnabled);
    glDisable(GL_TEXTURE_2D);

    glLineWidth(1.5f);
    glColor4f(1.f, 1.f, 1.f, 1.f); 

    glBegin(GL_LINES);

    // Vertical lines
    for (float x = startX; x <= gridRight; x += m_cellSize) {
        // World to screen space transformation for start and end points
        glm::vec4 start(x, gridBottom, 0.0f, 1.0f);
        glm::vec4 end(x, gridTop, 0.0f, 1.0f);

        start = projMat * viewMat4 * start;
        end = projMat * viewMat4 * end;

        glVertex2f(start.x, start.y);
        glVertex2f(end.x, end.y);
    }

    // Horizontal lines
    for (float y = startY; y <= gridTop; y += m_cellSize) {
        // World to screen space transformation for start and end points
        glm::vec4 start(gridLeft, y, 0.0f, 1.0f);
        glm::vec4 end(gridRight, y, 0.0f, 1.0f);

        start = projMat * viewMat4 * start;
        end = projMat * viewMat4 * end;

        glVertex2f(start.x, start.y);
        glVertex2f(end.x, end.y);
    }

    glEnd();

    // Restore texture state
    if (textureEnabled) {
        glEnable(GL_TEXTURE_2D);
    }

    // Restore OpenGL state
    glPopAttrib();
}