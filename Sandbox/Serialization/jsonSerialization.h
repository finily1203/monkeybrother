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
		virtual bool IsGood();

		template <typename T>
		void ReadObject(T&, std::string const&);

		virtual void ReadInt(int&, std::string const&);
		virtual void ReadFloat(float&, std::string const&);
		virtual void ReadString(std::string&, std::string const&);

		std::vector<std::string> GetEntityKeys();

	private:
		nlohmann::json jsonObj;
		std::ifstream fileStream;
	};

	template <typename T>
	void JSONSerializer::ReadObject(T& gameObj, std::string const& parentKey)
	{
		// initialize keyStream to the parentKey
		std::istringstream keyStream(parentKey);

		// holds each segment of the JSON key
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

		gameObj.Serialize(*this);
	}
}
