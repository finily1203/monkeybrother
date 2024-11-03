#pragma once
#ifdef _WIN32
#define NOMINMAX  // Tells Windows "don't define min/max"
#define WIN32_LEAN_AND_MEAN  // Tells Windows "only give me the basics"
#include <windows.h>  // Include Windows stuff first
#endif
