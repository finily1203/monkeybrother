#pragma once
/********************************************************************************************************
/*!
	\file		serialization.h
	\project	Mosscape
	\author		Ian Loi, ian.loi, 2301393
	\brief		serialization.h contains the BaseSerializer class, this is the main serializer class that
				can be used efficiently if we were to have different serialization modes in the future.
				This file contains 8 non-member functions that handles the reading and writing of different
				value types such as game objects, int, float and string.

All content © 2024 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/********************************************************************************************************/


#include <glm/glm.hpp>
#include <string>
#include <iostream>

namespace Serializer
{
	enum class SerializationMode
	{
		READ,		// 0
		WRITE		// 1
	};

	class BaseSerializer
	{
	public:
		virtual bool Open(std::string const&) = 0;
		virtual bool Save(std::string const&) = 0;
		virtual bool IsGood() = 0;
		virtual void ReadInt(int&, std::string const&) = 0;
		virtual void ReadFloat(float&, std::string const&) = 0;
		virtual void ReadString(std::string&, std::string const&) = 0;

		virtual void WriteInt(int&, std::string const&) = 0;
		virtual void WriteFloat(float&, std::string const&) = 0;
		virtual void WriteString(std::string&, std::string const&) = 0;
	};

	// READING from JSON file
	// template function that can be used to read data of different object type from
	// the JSON file
	template <typename T>
	inline void ReadObjectStream(BaseSerializer& fileStream, T& objInstanceType, SerializationMode mode)
	{
		objInstanceType.Serialize(fileStream, mode);
	}

	inline void ReadObjectStream(BaseSerializer& fileStream, int& data, std::string const& jsonKey)
	{
		fileStream.ReadInt(data, jsonKey);
	}

	inline void ReadObjectStream(BaseSerializer& fileStream, float& data, std::string const& jsonKey)
	{
		fileStream.ReadFloat(data, jsonKey);
	}

	inline void ReadObjectStream(BaseSerializer& fileStream, std::string& data, std::string const& jsonKey)
	{
		fileStream.ReadString(data, jsonKey);
	}

	// WRITING to JSON file
	// template function that can be used to write data of different object type to
	// the JSON file
	template <typename T>
	inline void WriteObjectStream(BaseSerializer& fileStream, T& objInstanceType, SerializationMode mode)
	{
		objInstanceType.Serialize(fileStream, mode);
	}

	inline void WriteObjectStream(BaseSerializer& fileStream, int& data, std::string const& jsonKey)
	{
		fileStream.WriteInt(data, jsonKey);
	}

	inline void WriteObjectStream(BaseSerializer& fileStream, float& data, std::string const& jsonKey)
	{
		fileStream.WriteFloat(data, jsonKey);
	}

	inline void WriteObjectStream(BaseSerializer& fileStream, std::string& data, std::string const& jsonKey)
	{
		fileStream.WriteString(data, jsonKey);
	}
}
