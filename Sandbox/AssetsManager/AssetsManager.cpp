/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   AssetsManager.cpp
@brief:  This source file includes all the implementation of the AssetsManager class.
         AssetsManager class is a child class of GameSystems, which is responsible for
         the all the assets of the game. It uses serialization to load all the assets.
		 Currently the assets that are loaded are: textures, shaders, fonts and audio.

         Joel Chu (c.weiyuan): Implemented all of the functions that belongs to
                               the AssetsManager class.
                               100%
*//*___________________________________________________________________________-*/
#include "AssetsManager.h"
#include "stb_image.h"
#include "fmod.hpp"
#include "GlobalCoordinator.h"

#include <filesystem>

#include <ft2build.h>
#include FT_FREETYPE_H


AssetsManager::AssetsManager() : audSystem(nullptr), m_textureWidth(0), m_textureHeight(0), nrChannels(0), hasAssetsListChanged(false) {}

AssetsManager::~AssetsManager()
{
    ClearAudio();
}

//leave empty for now
void AssetsManager::initialise()
{
    LoadShaderAssets();
    LoadTextureAssets();
    LoadFontAssets();
    LoadAudioAssets();
}

void AssetsManager::update()
{
}

void AssetsManager::cleanup()
{
}

SystemType AssetsManager::getSystem()
{
	return SystemType::AssetsManagerType;
}

//-----------------------------TEXTURE ASSETS----------------------------------//
void AssetsManager::LoadTextureAssets() const {
    std::string jsonFilePath = FilePathManager::GetAssetsJSONPath();
    std::ifstream file(jsonFilePath);
    nlohmann::json jsonObj;

    if (file.is_open())
    {
        file >> jsonObj;
        file.close();
    }

    for (const auto& textureAsset : jsonObj["textureAssets"])
    {
        std::string textureName = textureAsset["id"].get<std::string>();
        std::string relativePath = textureAsset["filePath"].get<std::string>();

        std::string textureFilePath = FilePathManager::GetExecutablePath() + "\\" + relativePath;
        assetsManager.LoadTexture(textureName, textureFilePath);
    }
}

void AssetsManager::LoadTexture(const std::string& texName, const std::string& texPath) {
    if (m_Textures.find(texName) != m_Textures.end()) {
		std::cerr << "Texture already loaded!" << std::endl;
		return;
	}

    //int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(texPath.c_str(), &m_textureWidth, &m_textureHeight, &nrChannels, 0);

    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;

        // Load texture into OpenGL
        GLuint texID;
        glGenTextures(1, &texID);
        glBindTexture(GL_TEXTURE_2D, texID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, m_textureWidth, m_textureHeight, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Set texture parameters to prevent bleeding
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // Use nearest filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // Use nearest filtering

        stbi_image_free(data);

        m_Textures[texName] = texID;
        m_AssetList.push_back(texName);
        std::cout << "texture loaded successfully!" << std::endl;
    }
    else {
        std::cerr << "Failed to load texture!" << std::endl;
        stbi_image_free(data);
        return;
    }
}

GLuint AssetsManager::GetTexture(const std::string& texName) const {
    auto iterator = m_Textures.find(texName);
    if (iterator != m_Textures.end()) {
		return iterator->second;
	}
    else {
		std::cerr << "Texture not found!" << std::endl;
		return 0;
	}
}

void AssetsManager::UnloadTexture(const std::string& texName) {
    auto iterator = m_Textures.find(texName);
    if (iterator != m_Textures.end()) {
		glDeleteTextures(1, &iterator->second);
		m_Textures.erase(iterator);
        std::cout << "Texture unloaded successfully!" << std::endl;
	}
    else {
        std::cerr << "Texture not found!" << std::endl;
    }
}

void AssetsManager::ClearTextures() {
    for (auto& texture : m_Textures) {
		glDeleteTextures(1, &texture.second);
	}
	m_Textures.clear();
	std::cout << "All textures cleared!" << std::endl;
}

int AssetsManager::texWidthGet() {
    return m_textureWidth;
}
int AssetsManager::texHeightGet() {
    return m_textureHeight;
}

int AssetsManager::nrChannelsGet() {
	return nrChannels;
}

//-----------------------------SHADER ASSETS----------------------------------//
void AssetsManager::LoadShaderAssets() const {
    std::string jsonFilePath = FilePathManager::GetAssetsJSONPath();
    std::ifstream file(jsonFilePath);
    nlohmann::json jsonObj;

    if (file.is_open())
    {
        file >> jsonObj;
        file.close();
    }

    for (const auto& shaderAsset : jsonObj["shaderAssets"])
    {
        std::string shaderName = shaderAsset["id"].get<std::string>();
        std::string relativePath = shaderAsset["filePath"].get<std::string>();

        std::string shaderFilePath = FilePathManager::GetExecutablePath() + "\\" + relativePath;
        assetsManager.LoadShader(shaderName, shaderFilePath);
    }
}

void AssetsManager::LoadShader(const std::string& name, const std::string& filePath) {
    if (m_Shaders.find(name) != m_Shaders.end()) {
        std::cerr << "Shader already loaded!" << std::endl;
        return;
    }

    ShaderProgramSource source = Shader::ParseShader(filePath);
    auto shader = std::make_unique<Shader>(source.VertexSource, source.FragmentSource);
    if (!shader->IsCompiled()) {
		std::cerr << "Shader compilation failed." << std::endl;
		return;
    }
    else {
        m_Shaders[name] = std::move(shader);
        m_AssetList.push_back(name);
        std::cout << "Shader loaded successfully!" << std::endl;
    }

}

Shader* AssetsManager::GetShader(const std::string& name) const {
    auto iterator = m_Shaders.find(name);
    if (iterator != m_Shaders.end()) {
        return iterator->second.get();
    }
    else {
		std::cerr << "Shader not found!" << std::endl;
		return nullptr;
	}
}

void AssetsManager::UnloadShader(const std::string& name) {
    auto iterator = m_Shaders.find(name);
    if (iterator != m_Shaders.end()) {
		m_Shaders.erase(iterator);
		std::cout << "Shader unloaded successfully!" << std::endl;
	}
    else {
		std::cerr << "Shader not found!" << std::endl;
	}
}

void AssetsManager::ClearShaders() {
	m_Shaders.clear();
	std::cout << "All shaders cleared!" << std::endl;
}


//-----------------------------AUDIO ASSETS----------------------------------//
void AssetsManager::LoadAudioAssets() {
    FMOD_RESULT result;

    result = FMOD::System_Create(&audSystem);
    if (result != FMOD_OK) {
        std::cout << "FMOD error! (" << result << ") " << std::endl;
        return;
    }

    result = audSystem->init(32, FMOD_INIT_NORMAL, nullptr);
    if (result != FMOD_OK) {
        std::cout << "FMOD error! (" << result << ") " << std::endl;
        return;
    }

    std::string jsonFilePath = FilePathManager::GetAssetsJSONPath();
    std::ifstream file(jsonFilePath);
    nlohmann::json jsonObj;

    if (file.is_open())
    {
        file >> jsonObj;
        file.close();
    }

    for (const auto& audioAsset : jsonObj["audioAssets"])
    {
        std::string audioName = audioAsset["id"].get<std::string>();
        std::string relativePath = audioAsset["filePath"].get<std::string>();

        std::string audioFilePath = FilePathManager::GetExecutablePath() + "\\" + relativePath;
        assetsManager.LoadAudio(audioName, audioFilePath, audSystem);
    }
}

void AssetsManager::LoadAudio(const std::string& songName, const std::string& filePath, FMOD::System* auSystem) {
    FMOD::Sound* audioSong = nullptr;
    FMOD_RESULT result = auSystem->createSound(filePath.c_str(), FMOD_DEFAULT, nullptr, &audioSong);
    if (result != FMOD_OK) {
        std::cout << "FMOD error! (" << result << ") " << std::endl;
        return;
    }

    m_Audio[songName] = audioSong;
    m_AssetList.push_back(songName);
}

FMOD::Sound* AssetsManager::GetAudio(const std::string& name) const {
    auto iterator = m_Audio.find(name);
    if (iterator != m_Audio.end()) {
		return iterator->second;
	}
    else {
		std::cerr << "Audio not found!" << std::endl;
		return 0;
	}
}

FMOD::System* AssetsManager::GetAudioSystem() const {
	return audSystem;
}

void AssetsManager::UnloadAudio(const std::string& name) {
    auto iterator = m_Audio.find(name);
    if (iterator != m_Audio.end()) {
        iterator->second->release();
        std::cout << "Audio unloaded successfully!" << std::endl;
    }
    else {
        std::cout << "Audio not found!" << std::endl;
    }
}

void AssetsManager::ClearAudio() {
    for (auto& audio : m_Audio) {
		audio.second->release();
	}
	m_Audio.clear();
    if (audSystem) {
        audSystem->close();
        audSystem->release();
        audSystem = nullptr;
    }
	std::cout << "All audio cleared!" << std::endl;
}


//-----------------------------FONT ASSETS----------------------------------//
void AssetsManager::LoadFontAssets() const {
    std::string jsonFilePath = FilePathManager::GetAssetsJSONPath();
    std::ifstream file(jsonFilePath);
    nlohmann::json jsonObj;

    if (file.is_open())
    {
        file >> jsonObj;
        file.close();
    }

    for (const auto& fontAssets : jsonObj["fontAssets"])
    {
        std::string fontName = fontAssets["id"].get<std::string>();
        std::string relativePath = fontAssets["filePath"].get<std::string>();

        std::string fontFilePath = FilePathManager::GetExecutablePath() + "\\" + relativePath;
        assetsManager.LoadFont(fontName, fontFilePath, 48 ); //default font size
        
    }
}

void AssetsManager::LoadFont(const std::string& fontName, const std::string& fontPath, unsigned int fontSize) {
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
       
        m_Fonts[fontName] = std::move(tempCharacters);
        m_FontPaths[fontName] = fontPath;
        m_AssetList.push_back(fontName);
    }
    else {
        std::cerr << "ERROR: Not all glyphs were loaded for font: " << fontPath << std::endl;
    }
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    std::cout << "Font loaded successfully: " << fontPath << " with total glyphs loaded: " << m_Fonts[fontName].size() << std::endl;
}

std::map<char, Character> AssetsManager::GetFont(const std::string& fontPath) const {
	auto iterator = m_Fonts.find(fontPath);
    if (iterator != m_Fonts.end()) {
		return iterator->second;
	}
    else {
		std::cerr << "Font not found!" << std::endl;
		return std::map<char, Character>();
	}
}

std::string AssetsManager::GetFontPath(const std::string& fontName) const {
    auto iterator = m_FontPaths.find(fontName);
    if (iterator != m_FontPaths.end()) {
		return iterator->second;
	}
    else {
		std::cerr << "Font not found!" << std::endl;
		return "";
	}
}

void AssetsManager::UnloadFont(const std::string& fontPath) {
	auto iterator = m_Fonts.find(fontPath);
    if (iterator != m_Fonts.end()) {
        for (auto& character : iterator->second) {
			glDeleteTextures(1, &character.second.TextureID);
		}
		m_Fonts.erase(iterator);
		std::cout << "Font unloaded successfully!" << std::endl;
	}
    else {
		std::cerr << "Font not found!" << std::endl;
	}
}

void AssetsManager::ClearFonts() {
    for (auto& font : m_Fonts) {
        for (auto& character : font.second) {
			glDeleteTextures(1, &character.second.TextureID);
		}
	}
	m_Fonts.clear();
	std::cout << "All fonts cleared!" << std::endl;
}

void AssetsManager::handleDropFile(std::string filePath) {
    std::filesystem::path path(filePath);

    if (path.extension() == ".png" || path.extension() == ".jpg" || path.extension() == ".jpeg") {
		LoadTexture(path.filename().string(), path.string());
        std::cout << "Texture loaded from drag and drop!" << std::endl;
	}
    else if (path.extension() == ".shader") {
        LoadShader(path.filename().string(), path.string());
        std::cout << "Shader loaded from drag and drop!" << std::endl;
    }
    else if (path.extension() == ".ttf") {
		LoadFont(path.filename().string(), path.string(), 48);
        std::cout << "Font loaded from drag and drop!" << std::endl;
	}
    else if (path.extension() == ".wav" || path.extension() == ".mp3") {
		LoadAudio(path.filename().string(), path.string(), audSystem);
        std::cout << "Audio loaded from drag and drop!" << std::endl;
	}
    else {
		std::cerr << "File type not supported!" << std::endl;
	}

    hasAssetsListChanged = true;
}

bool AssetsManager::checkIfAssetListChanged() const {
	return hasAssetsListChanged;
}

std::vector<std::string> AssetsManager::getAssetList() const {
	return m_AssetList;
}


const std::map<std::string, GLuint>& AssetsManager::getTextureList() const {
	return m_Textures;
}

const std::map<std::string, std::unique_ptr<Shader>>& AssetsManager::getShaderList() const {
	return m_Shaders;
}

const std::map<std::string, FMOD::Sound*>& AssetsManager::getAudioList() const {
	return m_Audio;
}

const std::map<std::string, std::map<char, Character>>& AssetsManager::getFontList() const {
	return m_Fonts;
}