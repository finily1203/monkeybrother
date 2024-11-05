#ifndef FONTCOMPONENT_H
#define FONTCOMPONENT_H

#include <string>
#include <glm/glm.hpp>
#include "vector3D.h"

struct FontComponent {
    std::string text;
    float textScale;
    float textBoxWidth;
    myMath::Vector3D color;
    std::string fontId; 

    FontComponent() : textScale(0.f), textBoxWidth(0.f){}
};

#endif 
