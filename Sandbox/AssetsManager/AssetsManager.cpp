#include "AssetsManager.h"
#include "stb_image.h"
#include "AudioSystem.h"


AssetsManager::AssetsManager()
{
}

AssetsManager::~AssetsManager()
{
}

//leave empty for now
void AssetsManager::initialise()
{}

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
void AssetsManager::LoadTexture(const std::string& texName, const std::string& texPath) {
    if (m_Textures.find(texName) != m_Textures.end()) {
		std::cerr << "Texture already loaded!" << std::endl;
		return;
	}

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(texPath.c_str(), &width, &height, &nrChannels, 0);

    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;

        // Load texture into OpenGL
        GLuint texID;
        glGenTextures(1, &texID);
        glBindTexture(GL_TEXTURE_2D, texID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Set texture parameters to prevent bleeding
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // Use nearest filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // Use nearest filtering

        stbi_image_free(data);

        m_Textures[texName] = texID;
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

//-----------------------------SHADER ASSETS----------------------------------//
void AssetsManager::LoadShader(const std::string& name, const std::string& verPath, const std::string& fragPath) {
    if (m_Shaders.find(name) != m_Shaders.end()) {
        std::cerr << "Shader already loaded!" << std::endl;
        return;
    }

    ShaderProgramSource source = Shader::ParseShader(fragPath);
    auto shader = std::make_unique<Shader>(source.VertexSource, source.FragmentSource);
    if (!shader->IsCompiled()) {
		std::cerr << "Shader compilation failed." << std::endl;
		return;
    }
    else {
        m_Shaders[name] = std::move(shader);
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
void AssetsManager::LoadAudio(const std::string& songName, const std::string& filePath, FMOD::System* audioSystem) {
    FMOD::Sound* audioSong = nullptr;
    FMOD_RESULT result = audioSystem->createSound(filePath.c_str(), FMOD_DEFAULT, nullptr, &audioSong);
    if (result != FMOD_OK) {
        std::cout << "FMOD error! (" << result << ") " << std::endl;
        return;
    }

    m_Audio[songName] = audioSong;
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
	std::cout << "All audio cleared!" << std::endl;
}