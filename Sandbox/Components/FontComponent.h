/*
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Javier Chua (javierjunliang.chua)
@team   :  MonkeHood
@course :  CSD2401
@file   :  FontComponent.h
@brief  :  Defines the FontComponent struct used to manage text rendering properties for entities in the ECS.
           This component holds the necessary text data, scaling, color, and font information.

* Javier Chua (javierjunliang.chua) :
        - Created FontComponent struct to store and manage individual text data for entities.
        - Added members for text content, text scaling, color (using a custom Vector3D), and font ID.
        - Set a default constructor initializing text scale to zero for clear state definition.

File Contributions: Javier Chua
_______________________________________________________________________________________________________________*/

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
