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
    //textShader->Unbind(); // Unbind the shader

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




void FontSystem::loadFont(unsigned int fontSize) {
    std::string fontPath = "Graphics/Assets/Antonio.ttf"; 

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


    for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cerr << "ERROR::FREETYPE: Failed to load Glyph for character: " << c << std::endl;
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

        Characters.insert(std::pair<char, Character>(c, character));
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    ft = nullptr;
    face = nullptr;
    std::cout << "Font loaded successfully. Total glyphs loaded: " << Characters.size() << std::endl;
}



float FontSystem::measureTextWidth(const std::string& text, float scale) {
    float width = 5.0f;
    for (char c : text) {
        if (Characters.find(c) != Characters.end()) {
            width += (Characters[c].Advance >> 6) * scale; // Advance is in 1/64th of pixels
        }
    }
    return width;
}

std::vector<std::string> FontSystem::wrapText(const std::string& text, float maxWidth, float scale) {
    std::vector<std::string> lines;
    std::string currentLine;
    float spaceWidth = measureTextWidth(" ", scale); // Measure space width for word wrapping

    for (const char& c : text) {
        currentLine += c; // Add character to current line
        float lineWidth = measureTextWidth(currentLine, scale); // Measure line width

       
        if (lineWidth > maxWidth) {
          
            currentLine.pop_back(); 
            lines.push_back(currentLine); 
            currentLine = c; 
        }
    }

    // Add the last line if it's not empty
    if (!currentLine.empty()) {
        lines.push_back(currentLine);
    }

    return lines;
}

void FontSystem::renderWrappedText(const std::unique_ptr<Shader>& shader, const std::string& text, float x, float y, float scale, glm::vec3 color, float maxWidth) {
    if (!isInitialized) {
        std::cerr << "ERROR: FontSystem not initialized!" << std::endl;
        return;
    }

    textShader->Bind();
    textShader->SetUniform3f("textColor", color.r, color.g, color.b);

    // Wrap the text into lines
    std::vector<std::string> lines = wrapText(text, maxWidth, scale);

    // Set the line spacing
    float lineSpacing = 20.0f;

    // Render each line
    for (const auto& line : lines) {
        float xpos = x; // Reset x position for each new line

        float lineHeight = 0.0f; // Reset line height for each line
        float lineWidth = 0.0f; // To calculate the total width of the line

        // Calculate the total width of the line
        for (char c : line) {
            Character ch = Characters[c];
            lineWidth += (ch.Advance >> 6) * scale; // Accumulate character widths
        }

      
        xpos -= lineWidth / 2.0f; // Shift to the left by half of the total line width

        // Render characters in the current line
        for (char c : line) {
            Character ch = Characters[c];
            float ypos = y - (ch.Size.y - ch.Bearing.y);// adjust bearing for chars
            float w = ch.Size.x * scale;
            float h = ch.Size.y * scale;

          
            if (h > lineHeight) {
                lineHeight = h;
            }

            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
            };

            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            xpos += (ch.Advance >> 6) * scale; // Move to the right for the next character
        }

      
        y -= lineHeight + lineSpacing; 
    }

    glBindVertexArray(0);
    textShader->Unbind();
}








void FontSystem::draw(const std::string& text, float x, float y, float scale, glm::vec3 color, float maxWidth) {
    renderWrappedText(textShader, text, x, y, scale, color, maxWidth);
}



void FontSystem::cleanup() {
    std::cout << "Cleaning up FontSystem resources..." << std::endl;

    // FreeType cleanup
    for (auto& pair : Characters) {
        if (pair.second.TextureID != 0) {
            glDeleteTextures(1, &pair.second.TextureID);
            pair.second.TextureID = 0; // Avoid dangling pointers
        }
    }

    Characters.clear(); // Clear the character map

    if (VAO) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
    if (VBO) {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }

    isInitialized = false; // Reset initialization flag
    std::cout << "FontSystem cleanup successful." << std::endl;
}


void FontSystem::update() {
}
