#pragma once


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
