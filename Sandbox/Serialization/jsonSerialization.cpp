#include "jsonSerialization.h"


namespace Serializer
{
	bool JSONSerializer::Open(std::string const& filename)
	{
		fileStream.open(filename.c_str());

		if (fileStream.is_open())
		{
			fileStream >> jsonObj;
			fileStream.close();
			return true;
		}

		return false;
	}

	bool JSONSerializer::IsGood()
	{
		return !jsonObj.empty();
	}

	void JSONSerializer::ReadInt(int& data, std::string const& jsonKey)
	{
		// initialize keyStream to the jsonKey
		std::istringstream keyStream(jsonKey);

		// holds each segment of the JSON key
		std::string keySegment;
		nlohmann::json currentObj = jsonObj;

		// this will read each of the key in the parentKey one by one
		while (std::getline(keyStream, keySegment, '.'))
		{
			if (currentObj.contains(keySegment))
			{
				currentObj = currentObj[keySegment];
			}

			else
			{
				return;
			}
		}

		data = currentObj.get<int>();
	}

	void JSONSerializer::ReadFloat(float& data, std::string const& jsonKey)
	{
		// initialize keyStream to the jsonKey
		std::istringstream keyStream(jsonKey);

		// holds each segment of the JSON key
		std::string keySegment;
		nlohmann::json currentObj = jsonObj;

		// this will read each of the key in the parentKey one by one
		while (std::getline(keyStream, keySegment, '.'))
		{
			if (currentObj.contains(keySegment))
			{
				currentObj = currentObj[keySegment];
			}

			else
			{
				return;
			}
		}

		data = currentObj.get<float>();
	}

	void JSONSerializer::ReadString(std::string& data, std::string const& jsonKey)
	{
		// initialize keyStream to the jsonKey
		std::istringstream keyStream(jsonKey);

		// holds each segment of the JSON key
		std::string keySegment;
		nlohmann::json currentObj = jsonObj;

		// this will read each of the key in the parentKey one by one
		while (std::getline(keyStream, keySegment, '.'))
		{
			if (currentObj.contains(keySegment))
			{
				currentObj = currentObj[keySegment];
			}

			else
			{
				return;
			}
		}

		data = currentObj.get<std::string>();
	}

	std::vector<std::string> JSONSerializer::GetEntityKeys()
	{
		std::vector<std::string> allKeys;

		for (auto& element : jsonObj.items())
		{
			allKeys.push_back(element.key());
		}

		return allKeys;
	}
}