/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   EngineDefinitions.h
@brief:  This header file has some simple definitions for the engine
		 Joel Chu (c.weiyuan): Declared variables needed for engine
							   100%
*//*___________________________________________________________________________-*/

#pragma once
#ifdef _WIN32
#define NOMINMAX  // Tells Windows "don't define min/max"
#define WIN32_LEAN_AND_MEAN  // Tells Windows "only give me the basics"
#include <windows.h>  // Include Windows stuff first
#endif
