#pragma once
/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author  :  Ian Loi (ian.loi)
@team    :  MonkeHood
@course  :  CSD2401
@file    :  serialization.h
@brief   :  serialization.h contains the BaseSerializer class, this is the main serializer class that
			can be used efficiently if we were to have different serialization modes in the future.

*Ian Loi (ian.loi) :
		 - Created 8 non-member functions that handles the reading and writing of
		   different value types such as game objects, int, float and string.

File Contributions: Ian Loi (100%)

*//*__________________________________________________________________________________________________*/


#include <glm/glm.hpp>
#include <string>
#include <iostream>
#include "vector2D.h"
#include "matrix3x3.h"


class BaseSerializer
{
public:
	virtual bool Open(std::string const&) = 0;
	virtual bool Save(std::string const&) = 0;
	virtual bool IsGood() = 0;
	virtual void ReadBool(bool&, std::string const&) = 0;
	virtual void ReadInt(int&, std::string const&) = 0;
	virtual void ReadUnsignedInt(unsigned int&, std::string const&) = 0;
	virtual void ReadUnsignedLongLong(unsigned long long&, std::string const&) = 0;
	virtual void ReadFloat(float&, std::string const&) = 0;
	virtual void ReadDouble(double&, std::string const&) = 0;
	virtual void ReadString(std::string&, std::string const&) = 0;

	virtual void WriteBool(bool&, std::string const&, std::string const&) = 0;
	virtual void WriteInt(int&, std::string const&, std::string const&) = 0;
	virtual void WriteUnsignedInt(unsigned int&, std::string const&, std::string const&) = 0;
	virtual void WriteUnsignedLongLong(unsigned long long&, std::string const&, std::string const&) = 0;
	virtual void WriteFloat(float&, std::string const&, std::string const&) = 0;
	virtual void WriteDouble(double&, std::string const&, std::string const&) = 0;
	virtual void WriteString(std::string&, std::string const&, std::string const&) = 0;
};
