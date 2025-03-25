/*
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Owen Lew (z.lew)
@team   :  MonkeHood
@course :  CSD2401
@file   :  GUIGridSystem.h
@brief  :  This file contains the function declaration of the grid system in the debug system

*Owen Lew (z.lew):
        - Integrated a tilegrid system for the debug system
        - Users can now see the grid in the debug system
        - tilegrid can be used to place tiles / platforms in the game

File Contributions: Owen Lew (100%)

/*_______________________________________________________________________________________________________________*/

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