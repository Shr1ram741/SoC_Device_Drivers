#pragma once
#include <vector>
#include <cstdint>
#include <unordered_map>
#include "Config.h"
#include "Memory.hpp"
#include "MMU.hpp"

using namespace std

class Memory {
  friend class MMU;
  public:
    Memory(Config& cf): nextAvailableFrame(0), config(cf) {};
}
