#ifndef FONTCOMPONENT_H
#define FONTCOMPONENT_H

#include <string>
#include <glm/glm.hpp>

struct FontComponent {
    std::string text;    
    glm::vec2 position;  
    float scale;         
    glm::vec3 color;     
};

#endif 
