/*
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Javier Chua (javierjunliang.chua)
@team   :  MonkeHood
@course :  CSD2401
@file   :  GraphicsSystem.cpp
@brief  :  This file contains the implementation of the FontSystem class,
           the FontSystem class is responsible for rendering the font.

* Javier Chua (javierjunliang.chua) :
 Implemented the FontSystem class, including initialization of VAO, VBO, and blending settings.
        - Designed the initialise function to set up orthographic projection and prepare the font shader.
        - Developed the loadFont function to load and configure character glyphs using FreeType.
        - Created renderText function to handle word wrapping, line spacing, and character positioning.
        - Implemented cleanup to release allocated resources for fonts and graphics buffers.

 File Contributions:  Javier Chua 
/*_______________________________________________________________________________________________________________*/
#include "FontSystem.h"
#include "GlobalCoordinator.h"
#include <iostream>
#include <ft2build.h>
#include FT_FREETYPE_H

FontSystem::FontSystem() : VAO(0), VBO(0), isInitialized(false), projectionMatrix(1.0f) {
    if (isInitialized) return;
}

// Destructor
FontSystem::~FontSystem() {
   
}



void FontSystem::initialise() {
    if (isInitialized) return;
    std::cout << "FontSystem initialized" << std::endl;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    projectionMatrix = glm::ortho(
        -GLFWFunctions::windowWidth / 2.0f,  // left
        GLFWFunctions::windowWidth / 2.0f,   // right
        -GLFWFunctions::windowHeight / 2.0f, // bottom
        GLFWFunctions::windowHeight / 2.0f   // top
    );



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
    std::map<char, Character> tempCharacters;

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




void FontSystem::renderText(const std::string& fontId, const std::string& text, float x, float y, float scale, myMath::Vector3D color, float maxWidth, myMath::Matrix3x3 viewMat) {
    if (!isInitialized) {
        std::cerr << "ERROR: FontSystem not initialized!" << std::endl;
        return;
    }

    auto it = assetsManager.m_Fonts.find(fontId);
    if (it == assetsManager.m_Fonts.end()) {
		std::cerr << "ERROR: Font not loaded: " << fontId << std::endl;
		return;
	}

    const std::map<char, Character>& characters = it->second;

    glm::mat3 viewMatrix = myMath::Matrix3x3::ConvertToGLMMat3(viewMat);


    glm::mat4 viewMat4{ 1 };
    // Combine the projection and view matrices
    glm::mat4 finalProjection = projectionMatrix * viewMat4;

    // Bind the font shader and set uniforms
    assetsManager.GetShader("fontShader")->Bind();
    assetsManager.GetShader("fontShader")->SetUniform3f("textColor", color.GetX(), color.GetY(), color.GetZ());
    assetsManager.GetShader("fontShader")->SetUniformMatrix4f("projection", finalProjection);
    

    float xpos = x;
    float ypos = y;

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

   
    const float lineSpacing = 1.5f; 

    std::string word; 
    for (char c : text) {
        if (c == ' ') { 
            if (!word.empty()) {
               
                float wordWidth = 0.0f;
                for (char wc : word) {
                    if (characters.find(wc) != characters.end()) {
                        wordWidth += (characters.at(wc).Advance >> 6) * scale; // Advance width
                    }
                }

             
                if (xpos + wordWidth > x + maxWidth) {
                    xpos = x; 
                    ypos -= (characters.at(word[0]).Size.y * scale) * lineSpacing; 
                }

                // Render the word
                for (char wc : word) {
                    if (characters.find(wc) != characters.end()) {
                        Character ch = characters.at(wc);
                        float h = ch.Size.y * scale;

                        float yposAdjusted = ypos - (ch.Size.y - ch.Bearing.y) * scale;
                        float w = ch.Size.x * scale;

                        float vertices[6][4] = {
                            { xpos,     yposAdjusted + h,   0.0f, 0.0f },
                            { xpos,     yposAdjusted,       0.0f, 1.0f },
                            { xpos + w, yposAdjusted,       1.0f, 1.0f },

                            { xpos,     yposAdjusted + h,   0.0f, 0.0f },
                            { xpos + w, yposAdjusted,       1.0f, 1.0f },
                            { xpos + w, yposAdjusted + h,   1.0f, 0.0f }
                        };

                        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
                        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
                        glDrawArrays(GL_TRIANGLES, 0, 6);

                        xpos += (ch.Advance >> 6) * scale; 
                    }
                }
                word.clear(); // Clear the word after rendering
            }
            xpos += 25; // Add space between words
            continue;
        }

        word += c; 
    }

    // Render the last word if there is one
    if (!word.empty()) {
        float wordWidth = 0.0f;
        for (char wc : word) {
            if (characters.find(wc) != characters.end()) {
                wordWidth += (characters.at(wc).Advance >> 6) * scale; // Advance width
            }
        }

        if (xpos + wordWidth > x + maxWidth) {
            xpos = x; // Reset x position to the start of the line
            ypos -= (characters.at(word[0]).Size.y * scale) * lineSpacing; 
        }

        for (char wc : word) {
            if (characters.find(wc) != characters.end()) {
                Character ch = characters.at(wc);
                float h = ch.Size.y * scale;

                float yposAdjusted = ypos - (ch.Size.y - ch.Bearing.y) * scale;
                float w = ch.Size.x * scale;

                float vertices[6][4] = {
                    { xpos,     yposAdjusted + h,   0.0f, 0.0f },
                    { xpos,     yposAdjusted,       0.0f, 1.0f },
                    { xpos + w, yposAdjusted,       1.0f, 1.0f },

                    { xpos,     yposAdjusted + h,   0.0f, 0.0f },
                    { xpos + w, yposAdjusted,       1.0f, 1.0f },
                    { xpos + w, yposAdjusted + h,   1.0f, 0.0f }
                };

                glBindTexture(GL_TEXTURE_2D, ch.TextureID);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
                glDrawArrays(GL_TRIANGLES, 0, 6);

                xpos += (ch.Advance >> 6) * scale; 
            }
        }
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    assetsManager.GetShader("fontShader")->Unbind();
}

void FontSystem::draw(const std::string& text, const std::string& fontId, float x, float y, float scale, myMath::Vector3D color, float maxWidth, myMath::Matrix3x3 viewMat) {
  
    renderText(fontId, text, x, y, scale, color, maxWidth, viewMat);
}

void FontSystem::cleanup() {
    if (!isInitialized) return;

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    //textShader.reset(); 

    assetsManager.ClearFonts();

    isInitialized = false;

}

void FontSystem::update() {
}

SystemType FontSystem::getSystem() {
	return FontType;
}