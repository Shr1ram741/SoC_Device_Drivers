#pragma once
#include <vector>
#include <cstdint>
#include <unordered_map>
#include "Config.h"
#include "Memory.hpp"
#include "MMU.hpp"

struct MMU {
  public:
    MMU(const Memory& memory, const Config& cfg): mem(Memory), config(cfg) {};
    void storeVal(uint64_t vaddr, uint64_t val) {
      uint64_t offsetBits = 0;
      uint64_t temp = config.pageSize;
      while (temp > 1) {
          offsetBits++;
          temp >>= 1;
      }
  
      uint64_t offsetMask = (1ULL << offsetBits) - 1;
      uint64_t vpn = vaddr >> offsetBits;
      uint64_t offset = vaddr & offsetMask;
  
      PageTableEntry entry;
      try {
          entry = mem.getPageTableEntry(0, vpn);
      } catch (...) {
          PageTableEntry newEntry;
          newEntry.physicalFrame = mem.nextAvailableFrame;
          newEntry.present = true;
          newEntry.user = true;
          newEntry.write = true;
          mem.nextAvailableFrame += config.pageSize;
          mem.setPageTableEntry(0, vpn, newEntry);
          entry = newEntry;
      }
      uint64_t paddr = entry.physicalFrame + offset;
      mem.memory_contents[paddr] = val;
    }

    uint64_t loadVal(uint64_t vaddr) {
      uint64_t paddr = translate(vaddr);
      auto p_ent = mem.memory_contents.find(paddr);
      if (p_ent == mem.memory_contents.end()) {
          throw std::runtime_error("Invalid physical address");
      }
      return p_ent->second;
    }

    private:
    const Memory& mem;
    const Config& config;

    uint64_t translate(uint64_t vaddr) {
      uint64_t offsetBits = 0;
      uint64_t temp = config.pageSize;
      while (temp > 1) {
          offsetBits++;
          temp >>= 1;
      }
  
      uint64_t offsetMask = (1ULL << offsetBits) - 1;
      uint64_t vpn = vaddr >> offsetBits;
      uint64_t offset = vaddr & offsetMask;
  
      PageTableEntry entry;
      try {
          entry = mem.getPageTableEntry(0, vpn);
      } catch (...) {
          throw std::runtime_error("Virtual address not mapped");
      }
  
      uint64_t paddr = entry.physicalFrame + offset;
      return paddr;
    }
};
