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


FontSystem::FontSystem() :  VAO(0), VBO(0) {
  
}

// Destructor
FontSystem::~FontSystem() {
    std::cout << "clear font system" << std::endl;
    //cleanup();
}


void FontSystem::initialise() {
    std::cout << "FontSystem initialized" << std::endl;

  
    textShader = std::make_unique<Shader>(vertexShaderSource, fragmentShaderSource);
    if (!textShader->IsCompiled()) {
        std::cerr << "ERROR: Failed to create shader!" << std::endl;
        return;
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);

 
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void FontSystem::loadFont(unsigned int fontSize) {
    std::string fontPath = "Graphics/Assets/liberation-mono.ttf"; 

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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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

    std::cout << "Font loaded successfully. Total glyphs loaded: " << Characters.size() << std::endl;
}



void FontSystem::renderText(Shader* shader,const std::string& text, float x, float y, float scale, glm::vec3 color) {
    // Check if the shader is initialized
    if (!textShader) {
        std::cerr << "ERROR: textShader is not initialized! Cannot render text." << std::endl;
        return;
    }


    textShader->Bind();


    textShader->SetUniform3f("textColor", color.x, color.y, color.z);
    textShader->SetUniform1i("text", 0); // Assuming texture unit 0 is used

 
    glActiveTexture(GL_TEXTURE0);

    for (char c : text) {

        Character ch = Characters[c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale; 
        float h = ch.Size.y * scale; 

        
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f }, // Top left
            { xpos,     ypos,       0.0f, 1.0f }, // Bottom left
            { xpos + w, ypos,       1.0f, 1.0f }, // Bottom right
            { xpos,     ypos + h,   0.0f, 0.0f }, // Top left
            { xpos + w, ypos,       1.0f, 1.0f }, // Bottom right
            { xpos + w, ypos + h,   1.0f, 0.0f }  // Top right
        };

       
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);

      
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

      
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

   
        x += (ch.Advance >> 6) * scale; 
    }

  
    glBindVertexArray(0);

   
    textShader->Unbind();
}

void FontSystem::draw(const std::string& text, float x, float y, float scale, glm::vec3 color) {
    renderText(textShader.get(), text, x, y, scale, color);
}


void FontSystem::cleanup() {
    std::cout << "Cleaning up FontSystem resources..." << std::endl;


    if (VAO) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
    if (VBO) {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }


    for (auto& pair : Characters) {
        if (pair.second.TextureID != 0) {
            glDeleteTextures(1, &pair.second.TextureID);
        }
    }

    Characters.clear();

    std::cout << "FontSystem cleanup successfull." << std::endl;
}


void FontSystem::update() {
}
