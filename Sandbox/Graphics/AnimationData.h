#pragma once
#include <vector>
#include <glm/glm.hpp>  

class AnimationData {
public:
    AnimationData(int totalFrames, float frameDuration, int sheetColumns, int sheetRows);

    void Update(float deltaTime);
    const std::vector<glm::vec2>& GetCurrentUVs() const;
    int GetCurrentFrame() const;

private:
    int m_TotalFrames;
    float m_FrameDuration;
    int m_CurrentFrame;
    float m_ElapsedTime;

    int m_SheetColumns;
    int m_SheetRows;

    std::vector<glm::vec2> m_CurrentUVs;  // UV coordinates for the current frame

    void CalculateUVsForFrame(int frame);
};
