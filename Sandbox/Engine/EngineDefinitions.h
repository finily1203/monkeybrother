#pragma once
#ifdef _WIN32
#define NOMINMAX  // Prevent Windows.h from defining min/max macros
#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#endif
