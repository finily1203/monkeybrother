/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   ECSDefinitions.h
@brief:  This header file helps to define the variables / types used in the ECS
		 system. This is to ensure that the variables are consistent throughout
		 the ECS system.

		 Joel Chu (c.weiyuan): declared the properties for ECS class
							   100%
*//*___________________________________________________________________________-*/

#pragma once

#include <queue>
#include <array>
#include <bitset>
#include <string>
#include <unordered_map>
#include <string>
#include <cassert>
#include <iostream>
#include <set>
using Entity = std::uint32_t;
using ComponentSig = std::bitset<32>; //Sig for Signature // for now set to 32 components
using ComponentType = std::uint8_t;

constexpr Entity MAX_ENTITIES = 5000;
constexpr ComponentType MAX_COMPONENTS = 32;