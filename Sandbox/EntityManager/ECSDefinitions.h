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