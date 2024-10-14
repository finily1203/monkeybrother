/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Ian Loi (ian.loi)
@team   :  MonkeHood
@course :  CSD2401
@file   :  jsonSerialization.cpp
@brief  :  jsonSerialization.cpp handles basic serialization/deserialization operations.

*Ian Loi (ian.loi) :
		- Create functions that open a JSON file for deserialization, save to JSON file 
		  for serialization and all the operations of reading/writing of int, float and string.

File Contributions: Ian Loi (100%)
				 
*//*__________________________________________________________________________________________________*/


#include "jsonSerialization.h"


bool JSONSerializer::Open(std::string const& filename)
{
	// opening the JSON file
	fileStream.open(filename.c_str());

	// check if the file was opened successfully
	if (fileStream.is_open())
	{
		// read the contents of the JSON file into the JSON object
		fileStream >> jsonObj;
			
		// close the JSON file
		fileStream.close();

		// returns true indicating JSON file opened successfully
		return true;
	}

	// returns false indicating JSON file could not be opened
	return false;
}

bool JSONSerializer::Save(std::string const& filename)
{
	// create an output file stream for the JSON file (filename)
	std::ofstream outputFile(filename);

	// check if output JSON file was opened successfully
	if (outputFile.is_open())
	{
		// write the contents of the JSON object to the output JSON file
		outputFile << jsonObj.dump(4);

		// close the output JSON file
		outputFile.close();

		// returns true indicating the JSON object was successfully saved
		return true;
	}

	// returns false indicating the output JSON file could not be opened
	return false;
}

bool JSONSerializer::IsGood()
{
	// returns true if the JSON object is not empty
	return !jsonObj.empty();
}

void JSONSerializer::ReadSpecificObject(glm::vec2& object, nlohmann::json const& jsonObj)
{
	if (jsonObj.is_object() && jsonObj.contains("x") && jsonObj.contains("y"))
	{
		object.x = jsonObj["x"].get<float>();
		object.y = jsonObj["y"].get<float>();
	}
}

void JSONSerializer::ReadSpecificObject(glm::mat3& object, nlohmann::json const& jsonObj)
{
	if (jsonObj.is_array() && jsonObj.size() == 3)
	{
		for (int i{}; i < 3; ++i)
		{
			if (jsonObj[i].is_array() && jsonObj[i].size() == 3)
			{
				for (int j{}; j < 3; ++j)
				{
					object[i][j] = jsonObj[i][j].get<float>();
				}
			}
		}
	}
}

void JSONSerializer::ReadInt(int& data, std::string const& jsonKey)
{
	// string buffer that contains the sequence of characters of jsonKey
	std::istringstream keyStream(jsonKey);

	// holds each different key from the keyStream
	std::string keySegment;

	// currentObj is initialized to the root JSON object
	nlohmann::json currentObj = jsonObj;

	// this will read each of the character in keyStream and assign these
	// characters to keySegment with '.' as the delimitter
	while (std::getline(keyStream, keySegment, '.'))
	{
		// the JSON object contains the key from the key stored in keySegment
		// eg. my JSON object contains the key "position" the keySegment value
		// is "position" so this is true and it will execute this if statement
		if (currentObj.contains(keySegment))
		{
			// key exists, currentObj will now be pointing to nested JSON
			// object based on the key
			currentObj = currentObj[keySegment];
		}

		// the JSON object does not contain the key
		else
		{
			return;
		}
	}

	// initialize data with the value associated with the final key in currentObj
	data = currentObj.get<int>();
}

void JSONSerializer::ReadFloat(float& data, std::string const& jsonKey)
{
	// string buffer that contains the sequence of characters of jsonKey
	std::istringstream keyStream(jsonKey);

	// holds each different key from the keyStream
	std::string keySegment;

	// currentObj is initialized to the root JSON object
	nlohmann::json currentObj = jsonObj;

	// this will read each of the character in keyStream and assign these
	// characters to keySegment with '.' as the delimitter
	while (std::getline(keyStream, keySegment, '.'))
	{
		// the JSON object contains the key from the key stored in keySegment
		if (currentObj.contains(keySegment))
		{
			// key exists, currentObj will now be pointing to nested JSON
			// object based on the key
			currentObj = currentObj[keySegment];
		}

		else
		{
			return;
		}
	}

	// initialize data with the value associated with the final key in currentObj
	data = currentObj.get<float>();
}

void JSONSerializer::ReadString(std::string& data, std::string const& jsonKey)
{
	// string buffer that contains the sequence of characters of jsonKey
	std::istringstream keyStream(jsonKey);

	// holds each different key from the keyStream
	std::string keySegment;

	// currentObj is initialized to the root JSON object
	nlohmann::json currentObj = jsonObj;

	// this will read each of the character in keyStream and assign these
	// characters to keySegment with '.' as the delimitter
	while (std::getline(keyStream, keySegment, '.'))
	{
		// the JSON object contains the key from the key stored in keySegment
		if (currentObj.contains(keySegment))
		{
			// key exists, currentObj will now be pointing to nested JSON
			// object based on the key
			currentObj = currentObj[keySegment];
		}

		else
		{
			return;
		}
	}

	// initialize data with the value associated with the final key in currentObj
	data = currentObj.get<std::string>();
}

void JSONSerializer::WriteInt(int& data, std::string const& jsonKey)
{
	// string buffer that contains the sequence of characters of jsonKey
	std::istringstream keyStream(jsonKey);

	// holds each different key from the keyStream
	std::string keySegment;

	// pointer to the root of the JSON object
	nlohmann::json* currentObj = &jsonObj;

	// this will read each of the character in keyStream and assign these
	// characters to keySegment with '.' as the delimitter
	while (std::getline(keyStream, keySegment, '.'))
	{
		// Update the current JSON object pointer to point to the next segment
		currentObj = &((*currentObj)[keySegment]);
	}

	// assign the integer data to the current JSON object
	*currentObj = data;
}

void JSONSerializer::WriteFloat(float& data, std::string const& jsonKey)
{
	// string buffer that contains the sequence of characters of jsonKey
	std::istringstream keyStream(jsonKey);

	// holds each different key from the keyStream
	std::string keySegment;

	// pointer to the root of the JSON object
	nlohmann::json* currentObj = &jsonObj;

	// this will read each of the character in keyStream and assign these
	// characters to keySegment with '.' as the delimitter
	while (std::getline(keyStream, keySegment, '.'))
	{
		// Update the current JSON object pointer to point to the next segment
		currentObj = &((*currentObj)[keySegment]);
	}

	*currentObj = data;
}

void JSONSerializer::WriteString(std::string& data, std::string const& jsonKey)
{
	// holds each different key from the keyStream
	std::istringstream keyStream(jsonKey);

	// pointer to the root of the JSON object
	std::string keySegment;

	// pointer to the root of the JSON object
	nlohmann::json* currentObj = &jsonObj;

	// this will read each of the character in keyStream and assign these
	// characters to keySegment with '.' as the delimitter
	while (std::getline(keyStream, keySegment, '.'))
	{
		// Update the current JSON object pointer to point to the next segment
		currentObj = &((*currentObj)[keySegment]);
	}

	*currentObj = data;
}

nlohmann::json JSONSerializer::GetJSONObject() const
{
	return jsonObj;
}