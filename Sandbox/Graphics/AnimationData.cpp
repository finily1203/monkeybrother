#include "AnimationData.h"

AnimationData::AnimationData(int totalFrames, float frameDuration, int sheetColumns, int sheetRows)
    : m_TotalFrames(totalFrames), m_FrameDuration(frameDuration), m_CurrentFrame(0),
    m_ElapsedTime(0.0f), m_SheetColumns(sheetColumns), m_SheetRows(sheetRows) {
    // Calculate initial UVs for frame 0
    CalculateUVsForFrame(0);
}

void AnimationData::Update(float deltaTime) {
    m_ElapsedTime += deltaTime;

    if (m_ElapsedTime >= m_FrameDuration) {
        m_ElapsedTime = 0.0f;
        m_CurrentFrame = (m_CurrentFrame + 1) % m_TotalFrames;

        CalculateUVsForFrame(m_CurrentFrame); 
    }
}

const std::vector<glm::vec2>& AnimationData::GetCurrentUVs() const {
    return m_CurrentUVs;
}

int AnimationData::GetCurrentFrame() const {
    return m_CurrentFrame;
}

void AnimationData::CalculateUVsForFrame(int frame) {
   

  
    float uvWidth = 1.0f / m_SheetColumns;
    float uvHeight = 1.0f / m_SheetRows;

  
    float uMin = (frame % m_SheetColumns) * uvWidth ;
    float uMax = (frame % m_SheetColumns + 1) * uvWidth;
    float vMin = (frame / m_SheetColumns) * uvHeight;
    float vMax = (frame / m_SheetColumns + 1) * uvHeight;

    // Update the UV coordinates for the current frame
    m_CurrentUVs = {
        {uMax, vMax}, // top right
        {uMax, vMin}, // bottom right
        {uMin, vMin}, // bottom left
        {uMin, vMax}  // top left
    };
}



