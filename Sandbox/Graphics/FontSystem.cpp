#include "FontSystem.h"
#include <iostream>
#include <ft2build.h>
#include FT_FREETYPE_H


// Vertex Shader source code as a string
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;
uniform mat4 projection;

void main() {
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}
)";

// Fragment Shader source code as a string
const char* fragmentShaderSource = R"(
#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;

void main() {
    vec4 texColor = texture(text, TexCoords);
    color = vec4(textColor, texColor.r);
}
)";

FontSystem::FontSystem() : VAO(0), VBO(0), isInitialized(false), projectionMatrix(1.0f) {
    if (isInitialized) return;
}

// Destructor
FontSystem::~FontSystem() {
    // std::cout << "clear font system" << std::endl;
    cleanup();
}


void FontSystem::initialise() {
    if (isInitialized) return;
    std::cout << "FontSystem initialized" << std::endl;

    textShader = std::make_unique<Shader>(vertexShaderSource, fragmentShaderSource);
    if (!textShader || !textShader->isInitialized()) {
        std::cerr << "ERROR: Failed to create shader!" << std::endl;
        return;
    }
    else {
        std::cout << "Shader initialized successfully." << std::endl;
    }

    // Set up OpenGL state
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    projectionMatrix = glm::ortho(0.0f, 1600.0f, 0.0f, 900.0f);

    textShader->Bind(); // Bind the shader before setting uniforms
    textShader->SetUniformMatrix4f("projection", projectionMatrix); // Set the projection matrix
    textShader->Unbind(); // Unbind the shader

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    isInitialized = true;
}




void FontSystem::loadFont(const std::string& fontPath, unsigned int fontSize) {
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        std::cerr << "ERROR::FREETYPE: Could not initialize FreeType Library" << std::endl;
        return;
    }

    FT_Face face;
    if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) {
        std::cerr << "ERROR::FREETYPE: Failed to load font at path: " << fontPath << std::endl;
        FT_Done_FreeType(ft);
        return;
    }

    FT_Set_Pixel_Sizes(face, 0, fontSize);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    bool fontLoaded = true;
    std::map<char, Character> tempCharacters; // Temporary map for this font

    for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cerr << "ERROR::FREETYPE: Failed to load Glyph for character: " << c << std::endl;
            fontLoaded = false;
            continue;
        }

        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<unsigned int>(face->glyph->advance.x)
        };

        tempCharacters.insert(std::pair<char, Character>(c, character));
    }

    // Store this font's characters in the Fonts map
    if (fontLoaded) {
        Fonts[fontPath] = std::move(tempCharacters); // Move the local map into the Fonts map
    }
    else {
        std::cerr << "ERROR: Not all glyphs were loaded for font: " << fontPath << std::endl;
    }
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    std::cout << "Font loaded successfully: " << fontPath << " with total glyphs loaded: " << Fonts[fontPath].size() << std::endl;
}



void FontSystem::renderText(const std::unique_ptr<Shader>& shader, const std::string& fontPath, const std::string& text, float x, float y, float scale, glm::vec3 color, float maxWidth) {
    if (!isInitialized) {
        std::cerr << "ERROR: FontSystem not initialized!" << std::endl;
        return;
    }

    // Check if the font is loaded
    auto it = Fonts.find(fontPath);
    if (it == Fonts.end()) {
        std::cerr << "ERROR: Font not loaded: " << fontPath << std::endl;
        return;
    }

    const std::map<char, Character>& characters = it->second; // Use the specific font's character map

    textShader->Bind();
    textShader->SetUniform3f("textColor", color.r, color.g, color.b);

    float xpos = x; 
    float lineHeight = 0.0f; 

    // Render each character in the text
    for (char c : text) {
        if (characters.find(c) != characters.end()) {
            Character ch = characters.at(c);
            float ypos = y - (ch.Size.y - ch.Bearing.y) * scale; 
            float w = ch.Size.x * scale; // Scale width
            float h = ch.Size.y * scale; // Scale height

            // Prepare vertex data for the character
            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
            };

            // Render the character
            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            // Move to the right for the next character
            xpos += (ch.Advance >> 6) * scale;
        }
    }

    glBindVertexArray(0);
    textShader->Unbind();
}


void FontSystem::draw(const std::string& text, const std::string& fontPath, float x, float y, float scale, glm::vec3 color, float maxWidth) {
    renderText(textShader, fontPath, text, x, y, scale, color, maxWidth);
}


void FontSystem::cleanup() {
    if (!isInitialized) return;

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    textShader.reset(); // Free shader resources

    // Delete loaded fonts
    for (auto& pair : Fonts) {
        for (auto& character : pair.second) {
            glDeleteTextures(1, &character.second.TextureID); // Delete character textures
        }
    }
    Fonts.clear();
    isInitialized = false;
    std::cout << "FontSystem cleaned up." << std::endl;
}


void FontSystem::update() {
}