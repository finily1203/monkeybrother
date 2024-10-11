#pragma once
/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author  :  Ian Loi (ian.loi)
@team    :  MonkeHood
@course  :  CSD2401
@file    :  jsonSerialization.h
@brief   :  jsonSerialization.h contains the JSON Serialization class that inherits from the BaseSerializer class.

*Ian Loi (ian.loi) : 
		 - Create functions of ReadObject and WriteObject, ReadObject reads data from JSON file and assign 
		 to the particular entity's data, whereas WriteObject writes the entity's data to the JSON file.

File Contributions: Ian Loi (100%)

*//*__________________________________________________________________________________________________*/


#include "serialization.h"
#include "../Nlohmann/json.hpp"
#include <fstream>
#include <sstream>


namespace Serializer
{
	class JSONSerializer : public BaseSerializer
	{
	public:
		virtual bool Open(std::string const&);
		virtual bool Save(std::string const&);
		virtual bool IsGood();

		template <typename T>
		void ReadObject(T&, std::string const&);

		template <typename T>
		void WriteObject(T&, std::string const&);

		virtual void ReadInt(int&, std::string const&);
		virtual void ReadFloat(float&, std::string const&);
		virtual void ReadString(std::string&, std::string const&);

		virtual void WriteInt(int&, std::string const&);
		virtual void WriteFloat(float&, std::string const&);
		virtual void WriteString(std::string&, std::string const&);

		nlohmann::json GetJSONObject() const;

	private:
		nlohmann::json jsonObj;
		std::ifstream fileStream;
	};

	template <typename T>
	void JSONSerializer::ReadObject(T& gameObj, std::string const& parentKey)
	{
		// string buffer that contains the sequence of characters of parentKey
		std::istringstream keyStream(parentKey);

		// holds each different key from the keyStream
		std::string keySegment;
		nlohmann::json currentObj = jsonObj;

		// this read each of the key in parentKey one by one 
		// (eg. Player.pos.x -> Player will be the first key, followed by pos and so on)
		while (std::getline(keyStream, keySegment, '.'))
		{
			// finding the current key part in the JSON object
			auto it = currentObj.find(keySegment);

			if (it != currentObj.end())
			{
				// key is found, move deeper down into the nested JSON objects
				currentObj = *it;
			}

			else
			{
				return;
			}
		}

		if (std::is_same_v<T, glm::vec2>)
		{
			if (currentObj.is_object() && currentObj.contains("x") && currentObj.contains("y"))
			{
				gameObj.x = currentObj["x"].get<float>();
				gameObj.y = currentObj["y"].get<float>();
			}
		}
	
	}

	template <typename T>
	void JSONSerializer::WriteObject(T& gameObj, std::string const& parentKey)
	{
		// string buffer that contains the sequence of characters of parentKey
		std::istringstream keyStream(parentKey);
		
		// holds each different key from the keyStream
		std::string keySegment;
		nlohmann::json* currentObj = &jsonObj;

		// this read each of the key in parentKey one by one
		while (std::getline(keyStream, keySegment, '.'))
		{
			currentObj = &((*currentObj)[keySegment]);
		}

		(*currentObj)["x"] = gameObj.x;
		(*currentObj)["y"] = gameObj.y;
	}
}
