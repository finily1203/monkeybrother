/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   AssetsManager.h
@brief:  This header file includes all the declaration of the AssetsManager class.
		 Currently AssetsManager only handles fonts, shaders, textures and audio assets

		 Joel Chu (c.weiyuan): Declared all the functions in AssetsManager class.
							   100%
*//*___________________________________________________________________________-*/
#pragma once
#include "EngineDefinitions.h"
#include <map>
#include <memory>
#include <string>
#include "fmod.hpp"
#include "GraphicsSystem.h"

#include "FontSystem.h"



class AssetsManager : public GameSystems
{
public:
	AssetsManager();
	~AssetsManager();

	void initialise() override;
	void update() override;
	void cleanup() override;
	SystemType getSystem() override;

	//For loading textures
	void LoadTextureAssets() const;
	void LoadTexture(const std::string& name, const std::string& filePath);
	GLuint GetTexture(const std::string& name) const;
	void UnloadTexture(const std::string& name);
	void ClearTextures();

	int texWidthGet();
	int texHeightGet();
	int nrChannelsGet();

	//For loading shaders
	void LoadShaderAssets() const;
	void LoadShader(const std::string& name, const std::string& filePath);
	Shader* GetShader(const std::string& name) const;
	void UnloadShader(const std::string& name);
	void ClearShaders();


	//For loading audio
	void LoadAudioAssets();
	void LoadAudio(const std::string& name, const std::string& filePath, FMOD::System* audioSystem);
	FMOD::Sound* GetAudio(const std::string& name) const;
	FMOD::System* GetAudioSystem() const;
	void UnloadAudio(const std::string& name);
	void ClearAudio();


	//For loading fonts
	void LoadFontAssets() const;
	void LoadFont(const std::string& fontName, const std::string& fontPath, unsigned int fontSize);
	std::map<char, Character> GetFont(const std::string& fontName) const;
	std::string GetFontPath(const std::string& fontName) const;
	void UnloadFont(const std::string& fontPath);
	void ClearFonts();
	std::map<std::string, std::string> m_FontPaths;
	std::map<std::string, std::map<char, Character>> m_Fonts;

	//For Drag and Drop files from file explorer
	void handleDropFile(std::string filePath);
	bool checkIfAssetListChanged() const;
	std::vector<std::string> getAssetList() const;

	//For asset browser
	const std::map<std::string, GLuint>& getTextureList() const;
	const std::map<std::string, std::unique_ptr<Shader>>& getShaderList() const;
	const std::map<std::string, FMOD::Sound*>& getAudioList() const;
	const std::map<std::string, std::map<char, Character>>& getFontList() const;

private:
	FMOD::System* audSystem;

	std::map<std::string, GLuint> m_Textures;
	std::map<std::string, std::unique_ptr<Shader>> m_Shaders;
	std::map<std::string, FMOD::Sound*> m_Audio;

	std::vector<std::string> m_AssetList;

	int m_textureWidth, m_textureHeight, nrChannels;

	bool hasAssetsListChanged;
};
