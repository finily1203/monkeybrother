#pragma once


#include "serialization.h"
#include "../Nlohmann/json.hpp"
#include <fstream>
#include <vector>
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

		// read the data from JSON file to the game object
		gameObj.Serialize(*this, SerializationMode::READ);
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

		// write the game object data to the JSON file
		gameObj.Serialize(*this, SerializationMode::WRITE);
	}
}
