#ifndef FONTSYSTEM_H
#define FONTSYSTEM_H

#include <string>
#include <map>
#include <glm/glm.hpp>
#include "Shader.h"
#include "ECSCoordinator.h" // Include ECS Coordinator

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

    void loadFont(unsigned int fontSize); // Load a font
    void update() override;     // Update the FontSystem (if necessary)
    void initialise() override; // Initialize the FontSystem
    void cleanup() override;    // Clean up resources
    void draw(const std::string& text, float x, float y, float scale, glm::vec3 color); // Draw text

private:
    void renderText(Shader* shader,const std::string& text, float x, float y, float scale, glm::vec3 color); // Render text

    std::map<char, Character> Characters; // Map of characters
    unsigned int VAO, VBO;                 // Vertex Array Object and Vertex Buffer Object
    std::unique_ptr<Shader> textShader;         // Shader for rendering text
    bool isInitialized = false;
};

#endif // FONTSYSTEM_H
