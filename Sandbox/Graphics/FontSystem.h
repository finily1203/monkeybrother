/*
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Liu YaoTing (yaoting.liu), Javier Chua (javierjunliang.chua)
@team   :  MonkeHood
@course :  CSD2401
@file   :  FontSystem.h
@brief  :  This file contains the declaration of the FontSystem class, which
           is responsible for rendering the font in the game

* Javier Chua (javierjunliang.chua) :
*       - Implemented the declaration structure of the FontSystem class which inherits the GameSystems which includes member functions to initialise, update 
*         and render. Character structure is also implemented to encapsulate the features needed to render each character
*
* File Contributions: Javier Chua 
*
/*_ _ _ _ ________________________________________________________________________________-\*/
#ifndef FONTSYSTEM_H
#define FONTSYSTEM_H

#include <string>
#include <map>
#include <memory>
#include <glm/glm.hpp>
#include "Shader.h"
#include "Systems.h"


#include "vector3D.h"
#include "vector2D.h"
#include "matrix3x3.h"


struct Character {
    unsigned int TextureID;   // Texture ID for the character
    glm::ivec2 Size;          // Size of the glyph
    glm::ivec2 Bearing;       // Offset from the baseline to the top of the glyph
    unsigned int Advance;      // Horizontal offset to advance to the next glyph
};

class FontSystem : public GameSystems {
public:
    FontSystem();              // Constructor
    ~FontSystem();             // Destructor
    
    void update() override;     // Update the FontSystem (if necessary)
    void initialise() override; // Initialize the FontSystem
    void cleanup() override;    // Clean up resources
    SystemType getSystem() override;

    void loadFont(const std::string& fontPath, unsigned int fontSize); // Load a font

    void draw(const std::string& text, const std::string& fontId, float x, float y, float scale, myMath::Vector3D color, float maxWidth,  myMath::Matrix3x3 viewMat);
    bool isInitialized = false;
    std::map<std::string, std::map<char, Character>> Fonts;



private:
    void renderText(const std::string& fontId, const std::string& text, float x, float y, float scale, myMath::Vector3D color, float maxWidth, myMath::Matrix3x3 viewMat);
    glm::mat4 projectionMatrix;
    std::map<char, Character> Characters; // Map of characters
    unsigned int VAO, VBO;                 // Vertex Array Object and Vertex Buffer Object
    std::unique_ptr<Shader> textShader;         // Shader for rendering text
    bool isCleanedUp = false;


};

#endif // FONTSYSTEM_H
