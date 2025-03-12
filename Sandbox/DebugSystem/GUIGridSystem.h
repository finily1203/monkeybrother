#pragma once
#include <glm/glm.hpp>
#include "vector2D.h"
#include "matrix3x3.h"

class GridSystem {
public:
    GridSystem();
    ~GridSystem();

    void initialise(float cellSize, float gridWidth, float gridHeight);
    myMath::Vector2D snapToGrid(myMath::Vector2D position);
    void drawGrid();

    float getCellSize() const { return m_cellSize; }
    float getWidth() const { return m_gridWidth; }
    float getHeight() const { return m_gridHeight; }

private:
    float m_cellSize;
    float m_gridWidth;
    float m_gridHeight;
    bool m_isInitialized;
};