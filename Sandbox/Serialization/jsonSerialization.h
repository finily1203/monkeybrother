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


class JSONSerializer : public BaseSerializer
{
public:
	virtual bool Open(std::string const&);
	virtual bool Save(std::string const&);
	virtual bool IsGood();

	void ReadSpecificObject(myMath::Vector2D&, nlohmann::json const&);
	void ReadSpecificObject(glm::mat3&, nlohmann::json const&);
	void ReadSpecificObject(float&, nlohmann::json const&);
	void ReadSpecificObject(bool&, nlohmann::json const&);

	void WriteSpecificObject(myMath::Vector2D const&, nlohmann::json&);
	void WriteSpecificObject(glm::mat3 const&, nlohmann::json&);
	void WriteSpecificObject(float const&, nlohmann::json&);
	void WriteSpecificObject(bool const&, nlohmann::json&);

	template <typename T>
	void ReadObject(T&, std::string const&, std::string const&);

	template <typename T>
	void WriteObject(T&, std::string const&, std::string const&);

	virtual void ReadBool(bool&, std::string const&);
	virtual void ReadInt(int&, std::string const&);
	virtual void ReadUnsignedInt(unsigned int&, std::string const&);
	virtual void ReadUnsignedLongLong(unsigned long long&, std::string const&);
	virtual void ReadFloat(float&, std::string const&);
	virtual void ReadDouble(double&, std::string const&);
	virtual void ReadString(std::string&, std::string const&);
	virtual void ReadCharArray(char*, size_t, std::string const&);

	virtual void WriteBool(bool&, std::string const&, std::string const&);
	virtual void WriteInt(int&, std::string const&, std::string const&);
	virtual void WriteUnsignedInt(unsigned int&, std::string const&, std::string const&);
	virtual void WriteUnsignedLongLong(unsigned long long&, std::string const&, std::string const&);
	virtual void WriteFloat(float&, std::string const&, std::string const&);
	virtual void WriteDouble(double&, std::string const&, std::string const&);
	virtual void WriteString(std::string&, std::string const&, std::string const&);

	nlohmann::json GetJSONObject() const;

private:
	nlohmann::json jsonObject;
	std::ifstream fileStream;
};

template <typename T>
void JSONSerializer::ReadObject(T& gameObj, std::string const& entityId, std::string const& parentKey)
{
	// string buffer that contains the sequence of characters of parentKey
	std::istringstream keyStream(parentKey);

	// holds each different key from the keyStream
	std::string keySegment;
	nlohmann::json currentObj = jsonObject;

	while (std::getline(keyStream, keySegment, '.'))
	{
		// Check if we are at the "entities" key and need to handle the array
		if (keySegment == "entities")
		{
			// boolean to check if entityId is in the JSON object
			bool found = false;

			// loop through every entities in the array of the JSON object
			for (const auto& entity : currentObj["entities"])
			{
				// checks for the different entities in the array based on
				// the id of the entity
				if (entity["id"] == entityId)
				{
					// set the current object to be the current entity
					currentObj = entity;
					
					// set to true since the entity Id is found in the JSON object
					found = true;

					// stop looking through the JSON object since we found the entity
					break; 
				}
			}

			// If no matching entity was found, return early
			if (!found)
			{
				std::cout << "Entity with id " << entityId << " not found" << std::endl;
				return;
			}
		}
		else
		{
			// Continue the normal process for other keys
			auto it = currentObj.find(keySegment);
			if (it != currentObj.end())
			{
				// Key is found, move deeper down into the nested JSON objects
				currentObj = *it;
			}
			else
			{
				// Key not found, return early
				std::cout << "Key '" << keySegment << "' not found" << std::endl;
				return;
			}
		}
	}
	
	// this handles the reading of different types of objects
	// example will be a matrix3x3 object and a vector2D object can all
	// be read by calling this function
	ReadSpecificObject(gameObj, currentObj);
}

template <typename T>
void JSONSerializer::WriteObject(T& gameObj, std::string const& entityId, std::string const& parentKey)
{
	// string buffer that contains the sequence of characters of parentKey
	std::istringstream keyStream(parentKey);
		
	// holds each different key from the keyStream
	std::string keySegment;
	nlohmann::json* currentObj = &jsonObject;

	while (std::getline(keyStream, keySegment, '.'))
	{
		// Check if we are at the "entities" key and need to handle the array
		if (keySegment == "entities")
		{
			// boolean to check if entityId is in the JSON object
			bool found = false;

			// loop through every entities in the array of the JSON object
			for (auto& entity : (*currentObj)["entities"])
			{
				// checks for the different entities in the array based on
				// the id of the entity
				if (entity["id"] == entityId)
				{
					// set the current object to be the address of 
					// the current entity
					currentObj = &entity;

					// set to true since the entity Id is found in the JSON object
					found = true;

					// stop looking through the JSON object since we found the entity
					break;
				}
			}
		}

		else
		{
			// Continue the normal process for other keys
			currentObj = &((*currentObj)[keySegment]);
		}
	}

	// this handles the writing of different types of objects
	// example will be a matrix3x3 object and a vector2D object can all
	// be written by calling this function
	WriteSpecificObject(gameObj, *currentObj);
}
