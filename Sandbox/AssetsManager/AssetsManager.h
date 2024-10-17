#pragma once

#include <map>
#include <memory>
#include <string>
#include "GraphicsSystem.h"
#include "fmod.hpp"

class AssetsManager : public GameSystems
{
public:
	AssetsManager();
	~AssetsManager();

	void initialise() override;
	void update() override;
	void cleanup() override;

	//For loading textures
	void LoadTexture(const std::string& name, const std::string& filePath);
	GLuint GetTexture(const std::string& name) const;
	void UnloadTexture(const std::string& name);
	void ClearTextures();

	

	//For loading shaders
	void LoadShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath);
	Shader* GetShader(const std::string& name) const;
	void UnloadShader(const std::string& name);
	void ClearShaders();


	void LoadAudio(const std::string& name, const std::string& filePath, FMOD::System* audioSystem);
	FMOD::Sound* GetAudio(const std::string& name) const;
	void UnloadAudio(const std::string& name);
	void ClearAudio();


private:
	std::map<std::string, GLuint> m_Textures;
	std::map<std::string, std::unique_ptr<Shader>> m_Shaders;
	std::map<std::string, FMOD::Sound*> m_Audio;
};

//#include <unordered_map>
//#include <memory>
//#include <string>
//#include "../Nlohmann/json.hpp"
//#include <fstream>
//
//
//template <typename T>
//class AssestsManager
//{
//private:
//	std::unordered_map<std::string, std::shared_ptr<T>> assets;
//
//public:
//	std::shared_ptr<T> Load(std::string const& filePath);
//	void Unload(std::string const& filePath);
//	void Clear();
//};
//
//template<typename T>
//std::shared_ptr<T> AssestsManager<T>::Load(std::string const& filePath)
//{
//	if (assets.find(filePath) != assets.end())
//	{
//		return assets[filePath];
//	}
//}
//
//template<typename T>
//void AssestsManager<T>::Unload(std::string const& filePath)
//{
//	assets.erase(filePath);
//}
//
//template<typename T>
//void AssestsManager<T>::Clear()
//{
//	assets.clear();
//}
