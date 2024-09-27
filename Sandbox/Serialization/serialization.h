#pragma once


#include <string>
#include <iostream>

namespace Serializer
{
	class BaseSerializer
	{
	public:
		virtual bool Open(std::string const&) = 0;
		virtual bool IsGood() = 0;
		virtual void ReadInt(int&, std::string const&) = 0;
		virtual void ReadFloat(float&, std::string const&) = 0;
		virtual void ReadString(std::string&, std::string const&) = 0;
	};

	// template function that can be used to read and serialize data of different
	// object type
	template <typename T>
	inline void ReadObjectStream(BaseSerializer& fileStream, T& objInstanceType)
	{
		objInstanceType.Serialize(fileStream);
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
}
