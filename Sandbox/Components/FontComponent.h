#ifndef FONTCOMPONENT_H
#define FONTCOMPONENT_H

#include <string>
#include <glm/glm.hpp>
#include "vector3D.h"

struct FontComponent {
    std::string text;
    float textScale;
    MyMath::Vector3D color;
    std::string fontId; 
};

#endif 
