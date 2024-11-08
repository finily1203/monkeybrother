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


// this checks if the file can be opened successfully or not
bool JSONSerializer::Open(std::string const& filename)
{
	// opening the JSON file
	fileStream.open(filename.c_str());

	// check if the file was opened successfully
	if (fileStream.is_open())
	{
		// read the contents of the JSON file into the JSON object
		fileStream >> jsonObject;

		// close the JSON file
		fileStream.close();

		// returns true indicating JSON file opened successfully
		return true;
	}

	// returns false indicating JSON file could not be opened
	return false;
}

// this saves the data into the JSON file
bool JSONSerializer::Save(std::string const& filename)
{
	// create an output file stream for the JSON file (filename)
	std::ofstream outputFile(filename);

	// check if output JSON file was opened successfully
	if (outputFile.is_open())
	{
		// write the contents of the JSON object to the output JSON file
		outputFile << jsonObject.dump(2);

		// close the output JSON file
		outputFile.close();

		// returns true indicating the JSON object was successfully saved
		return true;
	}

	// returns false indicating the output JSON file could not be opened
	return false;
}

// ensures that the JSON object is not empty
bool JSONSerializer::IsGood()
{
	// returns true if the JSON object is not empty
	return !jsonObject.empty();
}

// read a Vector2D object from the JSON file
void JSONSerializer::ReadSpecificObject(myMath::Vector2D& object, nlohmann::json const& jsonObj)
{
	if (jsonObj.is_object() && jsonObj.contains("x") && jsonObj.contains("y"))
	{
		object.SetX(jsonObj["x"].get<float>());
		object.SetY(jsonObj["y"].get<float>());
	}
}

// read a Vector3D object from the JSON file
void JSONSerializer::ReadSpecificObject(myMath::Vector3D& object, nlohmann::json const& jsonObj)
{
	if (jsonObj.is_object() && jsonObj.contains("x") && jsonObj.contains("y") && jsonObj.contains("z"))
	{
		object.SetX(jsonObj["x"].get<float>());
		object.SetY(jsonObj["y"].get<float>());
		object.SetZ(jsonObj["z"].get<float>());
	}
}

// read a Matrix3x3 object from the JSON file
void JSONSerializer::ReadSpecificObject(myMath::Matrix3x3& object, nlohmann::json const& jsonObj)
{
	if (jsonObj.is_array() && jsonObj.size() == 3)
	{
		for (int i{}; i < 3; ++i)
		{
			if (jsonObj[i].is_array() && jsonObj[i].size() == 3)
			{
				for (int j{}; j < 3; ++j)
				{
					//object[i][j] = jsonObj[i][j].get<float>();
					float value = jsonObj[i][j].get<float>();
					object.SetMatrixValue(i, j, value);
				}
			}
		}
	}
}

// read a float object from the JSON file
void JSONSerializer::ReadSpecificObject(float& object, nlohmann::json const& jsonObj)
{
	if (jsonObj.is_number_float())
	{
		object = jsonObj.get<float>();
	}
}

// read a bool object from the JSON file
void JSONSerializer::ReadSpecificObject(bool& object, nlohmann::json const& jsonObj)
{
	if (jsonObj.is_boolean())
	{
		object = jsonObj.get<bool>();
	}
}

// read a string object from the JSON file
void JSONSerializer::ReadSpecificObject(std::string& object, nlohmann::json const& jsonObj)
{
	if (jsonObj.is_string())
	{
		object = jsonObj.get<std::string>();
	}
}

// write a Vector2D data to the JSON file
void JSONSerializer::WriteSpecificObject(myMath::Vector2D const& object, nlohmann::json& jsonObj)
{
	jsonObj["x"] = object.GetX();
	jsonObj["y"] = object.GetY();
}

// write a Vector3D data to the JSON file
void JSONSerializer::WriteSpecificObject(myMath::Vector3D const& object, nlohmann::json& jsonObj)
{
	jsonObj["x"] = object.GetX();
	jsonObj["y"] = object.GetY();
	jsonObj["z"] = object.GetZ();
}

// write a Matrix3x3 data to the JSON file
void JSONSerializer::WriteSpecificObject(myMath::Matrix3x3 const& object, nlohmann::json& jsonObj)
{
	jsonObj = nlohmann::json::array();

	for (int i{}; i < 3; ++i)
	{
		nlohmann::json row = nlohmann::json::array();

		for (int j{}; j < 3; ++j)
		{
			float value = object.GetMatrixValue(i, j);
			row.push_back(value);
		}

		jsonObj.push_back(row);
	}
}

// write a float object to the JSON file
void JSONSerializer::WriteSpecificObject(float const& object, nlohmann::json& jsonObj)
{
	jsonObj = object;
}

// write a bool object to the JSON file
void JSONSerializer::WriteSpecificObject(bool const& object, nlohmann::json& jsonObj)
{
	jsonObj = object;
}

// write a string object to the JSON file
void JSONSerializer::WriteSpecificObject(std::string const& object, nlohmann::json& jsonObj)
{
	jsonObj = object;
}

// read bool object
void JSONSerializer::ReadBool(bool& data, std::string const& jsonKey)
{
	// string buffer that contains the sequence of characters of jsonKey
	std::istringstream keyStream(jsonKey);

	// holds each different key from the keyStream
	std::string keySegment;

	// currentObj is initialized to the root JSON object
	nlohmann::json currentObj = jsonObject;

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
	data = currentObj.get<bool>();
}

// read an integer object
void JSONSerializer::ReadInt(int& data, std::string const& jsonKey)
{
	// string buffer that contains the sequence of characters of jsonKey
	std::istringstream keyStream(jsonKey);

	// holds each different key from the keyStream
	std::string keySegment;

	// currentObj is initialized to the root JSON object
	nlohmann::json currentObj = jsonObject;

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

// read unsigned int object
void JSONSerializer::ReadUnsignedInt(unsigned int& data, std::string const& jsonKey)
{
	// string buffer that contains the sequence of characters of jsonKey
	std::istringstream keyStream(jsonKey);

	// holds each different key from the keyStream
	std::string keySegment;

	// currentObj is initialized to the root JSON object
	nlohmann::json currentObj = jsonObject;

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

	data = currentObj.get<unsigned int>();
}

// read unsigned long long object
void JSONSerializer::ReadUnsignedLongLong(unsigned long long& data, std::string const& jsonKey)
{
	// string buffer that contains the sequence of characters of jsonKey
	std::istringstream keyStream(jsonKey);

	// holds each different key from the keyStream
	std::string keySegment;

	// currentObj is initialized to the root JSON object
	nlohmann::json currentObj = jsonObject;

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

	data = currentObj.get<unsigned long long>();
}

// read float object
void JSONSerializer::ReadFloat(float& data, std::string const& jsonKey)
{
	// string buffer that contains the sequence of characters of jsonKey
	std::istringstream keyStream(jsonKey);

	// holds each different key from the keyStream
	std::string keySegment;

	// currentObj is initialized to the root JSON object
	nlohmann::json currentObj = jsonObject;

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

// read double object
void JSONSerializer::ReadDouble(double& data, std::string const& jsonKey)
{
	// string buffer that contains the sequence of characters of jsonKey
	std::istringstream keyStream(jsonKey);

	// holds each different key from the keyStream
	std::string keySegment;

	// currentObj is initialized to the root JSON object
	nlohmann::json currentObj = jsonObject;

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
	data = currentObj.get<double>();
}

// read string object
void JSONSerializer::ReadString(std::string& data, std::string const& jsonKey)
{
	// string buffer that contains the sequence of characters of jsonKey
	std::istringstream keyStream(jsonKey);

	// holds each different key from the keyStream
	std::string keySegment;

	// currentObj is initialized to the root JSON object
	nlohmann::json currentObj = jsonObject;

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

// read a character array
void JSONSerializer::ReadCharArray(char* data, size_t maxSize, std::string const& jsonKey)
{
	std::istringstream keyStream(jsonKey);
	std::string keySegment;
	nlohmann::json currentObj = jsonObject;

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

	// Get the string from JSON
	std::string tempStr = currentObj.get<std::string>();

	// Copy characters to the buffer, respecting the size limit
	size_t length = std::min(tempStr.length(), maxSize);
	for (size_t i = 0; i < length; i++)
	{
		data[i] = tempStr[i];
	}
}

// write bool object to JSON file
void JSONSerializer::WriteBool(bool& data, std::string const& jsonKey, std::string const& filename)
{
	// string buffer that contains the sequence of characters of jsonKey
	std::istringstream keyStream(jsonKey);

	// holds each different key from the keyStream
	std::string keySegment;

	// pointer to the root of the JSON object
	nlohmann::json* currentObj = &jsonObject;

	// this will read each of the character in keyStream and assign these
	// characters to keySegment with '.' as the delimitter
	while (std::getline(keyStream, keySegment, '.'))
	{
		// Update the current JSON object pointer to point to the next segment
		currentObj = &((*currentObj)[keySegment]);
	}

	// assign the integer data to the current JSON object
	*currentObj = data;

	std::ofstream outFile(filename);
	if (outFile.is_open())
	{
		outFile << jsonObject.dump(2);
		outFile.close();
	}

	else
	{
		std::cout << "Error: could not open file " << filename << std::endl;
	}
}

// write an int object to JSON file
void JSONSerializer::WriteInt(int& data, std::string const& jsonKey, std::string const& filename)
{
	// string buffer that contains the sequence of characters of jsonKey
	std::istringstream keyStream(jsonKey);

	// holds each different key from the keyStream
	std::string keySegment;

	// pointer to the root of the JSON object
	nlohmann::json* currentObj = &jsonObject;

	// this will read each of the character in keyStream and assign these
	// characters to keySegment with '.' as the delimitter
	while (std::getline(keyStream, keySegment, '.'))
	{
		// Update the current JSON object pointer to point to the next segment
		currentObj = &((*currentObj)[keySegment]);
	}

	// assign the integer data to the current JSON object
	*currentObj = data;

	std::ofstream outFile(filename);
	if (outFile.is_open())
	{
		outFile << jsonObject.dump(2);
		outFile.close();
	}

	else
	{
		std::cout << "Error: could not open file " << filename << std::endl;
	}
}

// write unsigned int object JSON file
void JSONSerializer::WriteUnsignedInt(unsigned int& data, std::string const& jsonKey, std::string const& filename)
{
	// string buffer that contains the sequence of characters of jsonKey
	std::istringstream keyStream(jsonKey);

	// holds each different key from the keyStream
	std::string keySegment;

	// pointer to the root of the JSON object
	nlohmann::json* currentObj = &jsonObject;

	// this will read each of the character in keyStream and assign these
	// characters to keySegment with '.' as the delimitter
	while (std::getline(keyStream, keySegment, '.'))
	{
		// Update the current JSON object pointer to point to the next segment
		currentObj = &((*currentObj)[keySegment]);
	}

	*currentObj = data;

	std::ofstream outFile(filename);
	if (outFile.is_open())
	{
		outFile << jsonObject.dump(2);
		outFile.close();
	}

	else
	{
		std::cout << "Error: could not open file " << filename << std::endl;
	}
}

// write unsigned long long object to JSON file
void JSONSerializer::WriteUnsignedLongLong(unsigned long long& data, std::string const& jsonKey, std::string const& filename)
{
	// string buffer that contains the sequence of characters of jsonKey
	std::istringstream keyStream(jsonKey);

	// holds each different key from the keyStream
	std::string keySegment;

	// pointer to the root of the JSON object
	nlohmann::json* currentObj = &jsonObject;

	// this will read each of the character in keyStream and assign these
	// characters to keySegment with '.' as the delimitter
	while (std::getline(keyStream, keySegment, '.'))
	{
		// Update the current JSON object pointer to point to the next segment
		currentObj = &((*currentObj)[keySegment]);
	}

	*currentObj = data;

	std::ofstream outFile(filename);
	if (outFile.is_open())
	{
		outFile << jsonObject.dump(2);
	}

	else
	{
		std::cout << "Error: could not open file " << filename << std::endl;
	}
}

// write float object to JSON file
void JSONSerializer::WriteFloat(float& data, std::string const& jsonKey, std::string const& filename)
{
	// string buffer that contains the sequence of characters of jsonKey
	std::istringstream keyStream(jsonKey);

	// holds each different key from the keyStream
	std::string keySegment;

	// pointer to the root of the JSON object
	nlohmann::json* currentObj = &jsonObject;

	// this will read each of the character in keyStream and assign these
	// characters to keySegment with '.' as the delimitter
	while (std::getline(keyStream, keySegment, '.'))
	{
		// Update the current JSON object pointer to point to the next segment
		currentObj = &((*currentObj)[keySegment]);
	}

	*currentObj = data;

	std::ofstream outFile(filename);
	if (outFile.is_open())
	{
		outFile << jsonObject.dump(2);
	}

	else
	{
		std::cout << "Error: could not open file " << filename << std::endl;
	}
}

// write double object to JSON file
void JSONSerializer::WriteDouble(double& data, std::string const& jsonKey, std::string const& filename)
{
	// string buffer that contains the sequence of characters of jsonKey
	std::istringstream keyStream(jsonKey);

	// holds each different key from the keyStream
	std::string keySegment;

	// pointer to the root of the JSON object
	nlohmann::json* currentObj = &jsonObject;

	// this will read each of the character in keyStream and assign these
	// characters to keySegment with '.' as the delimitter
	while (std::getline(keyStream, keySegment, '.'))
	{
		// Update the current JSON object pointer to point to the next segment
		currentObj = &((*currentObj)[keySegment]);
	}

	*currentObj = data;

	std::ofstream outFile(filename);
	if (outFile.is_open())
	{
		outFile << jsonObject.dump(2);
	}

	else
	{
		std::cout << "Error: could not open file " << filename << std::endl;
	}
}

// write string to JSON file
void JSONSerializer::WriteString(std::string& data, std::string const& jsonKey, std::string const& filename)
{
	// holds each different key from the keyStream
	std::istringstream keyStream(jsonKey);

	// pointer to the root of the JSON object
	std::string keySegment;

	// pointer to the root of the JSON object
	nlohmann::json* currentObj = &jsonObject;

	// this will read each of the character in keyStream and assign these
	// characters to keySegment with '.' as the delimitter
	while (std::getline(keyStream, keySegment, '.'))
	{
		// Update the current JSON object pointer to point to the next segment
		currentObj = &((*currentObj)[keySegment]);
	}

	*currentObj = data;

	std::ofstream outFile(filename);
	if (outFile.is_open())
	{
		outFile << jsonObject.dump(2);
	}

	else
	{
		std::cout << "Error: could not open file " << filename << std::endl;
	}
}

// write character array to JSON file
void JSONSerializer::WriteCharArray(char* data, size_t maxSize, std::string const& jsonKey, std::string const& filename)
{
	// holds each different key from the keyStream
	std::istringstream keyStream(jsonKey);

	// pointer to the root of the JSON object
	std::string keySegment;

	// pointer to the root of the JSON object
	nlohmann::json* currentObj = &jsonObject;

	// this will read each of the character in keyStream and assign these
	// characters to keySegment with '.' as the delimitter
	while (std::getline(keyStream, keySegment, '.'))
	{
		// Update the current JSON object pointer to point to the next segment
		currentObj = &((*currentObj)[keySegment]);
	}

	std::string tempStr(data, std::min(strlen(data), maxSize));

	*currentObj = tempStr;

	std::ofstream outFile(filename);
	if (outFile.is_open())
	{
		outFile << jsonObject.dump(2);
		outFile.close();
	}
	else
	{
		std::cerr << "Error: could not write to file " << filename << std::endl;
	}
}

// return the JSON object 
nlohmann::json JSONSerializer::GetJSONObject() const
{
	return jsonObject;
}