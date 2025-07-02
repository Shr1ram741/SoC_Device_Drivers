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
    Memory(Config& cf): nextAvailableFrame(0) {
      pageTables.resize(cf.pageTablesLevels);
    }
  private:
    void setPageTableEntry(uint32_t level, uint32_t index, const PageTableEntry& entry) {
      if (level >= pageTables.size()) {
        throw runtime_error("Invalid Page Table Level");
      }
      pageTables[level][index]=entry;
    }

    PageTableEntry getPageTableEntry(uint32_t level, uint32_t index) const {
      if (level >= pageTables.size()) {
          throw std::runtime_error("Invalid Page Table Level");
      }
      auto PTE = pageTables[level].find(index);
      if (PTE == pageTables[level].end()) {
          throw std::runtime_error("Invalid Virtual Address");
      }
      else if (!(PTE->second.present)) {
          throw std::runtime_error("Page Fault");
      }
      return PTE->second;
    }
}
