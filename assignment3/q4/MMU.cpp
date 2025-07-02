#pragma once
#include <vector>
#include <cstdint>
#include <unordered_map>
#include "Config.h"
#include "Memory.hpp"
#include "MMU.hpp"

using namespace std

struct MMU {
  public:
    MMU(const Memory& memory, const Config& cfg): mem(Memory), config(cfg) {
      uint64_t frame = memory.nextAvailableFrame;
      memory.nextAvailableFrame += cfg.pageSize;
      CR3 = frame;
    }

    uint64_t encodeEntry(uint64_t frame, bool present = true, bool write = true, bool user = true) {
        uint64_t fpn = frame & ~0xFFFULL;
        uint64_t ispres = present ? 1 : 0;
        uint64_t iswrite = write ? 2 : 0;
        uint64_t usr = user ? 4 : 0;

        return (fpn | ispres | iswrite | usr)
    }
    
    void decodeEntry(uint64_t entry, uint64_t& frame, bool& present, bool& write, bool& user) {
        frame = entry & ~0xFFFULL;
        present = entry & 1;
        write = entry & 2;
        user = entry & 4;
    }

    void storeVal(uint64_t vaddr, uint64_t val) {
      uint64_t paddr = translate(vaddr,true);
      mem.mem_contents[paddr]=val;
    }

    uint64_t loadVal(uint64_t vaddr) {
      uint64_t paddr = translate(vaddr,false);
      auto p_ent = mem.memory_contents.find(paddr);
      if (p_ent == mem.memory_contents.end()) {
          throw runtime_error("Invalid physical address");
      }
      return p_ent->second;
    }

    private:
    const Memory& mem;
    const Config& config;
    unit64_t CR3;

    uint64_t translate(uint64_t vaddr, bool isWrite) {
      uint64_t offsetBits = 0;
      uint64_t temp = config.pageSize;
      while (temp > 1) {
          offsetBits++;
          temp >>= 1;
      }
  
      uint64_t offsetMask = (1ULL << offsetBits) - 1;
      uint64_t offset = vaddr & offsetMask;

      vector<uint64_t> page_indcs;
      uint64_t shift = offsetBits;
      
      for (int i = config.pageTableLevels - 1; i >= 0; --i) {
          uint64_t mask = (1ULL << config.bitsPerLevel[i]) - 1;
          page_indcs.insert(page_indcs.begin(), (vaddr >> shift) & mask);
          shift += config.bitsPerLevel[i];
      }
  
      uint64_t curr = CR3;
      for (int level = 0; level < config.pageTableLevels; ++level) {
          uint64_t index = indices[level];
          uint64_t entryAddr = curr + index * sizeof(uint64_t);
          uint64_t entry = 0;
  
          try {
              auto p_ent = memory_contents.find(entryAddr);
              if (it == memory_contents.end()) {throw runtime_error("Invalid physical address");}
              entry = p_ent->second;
          } catch (...) {
              if (level == config.pageTableLevels - 1 && !isWrite)
                  throw runtime_error("Page not mapped");
              uint64_t frame = mem.nextAvailableFrame;
              mem.nextAvailableFrame += config.pageSize;
              uint64_t newFrame = frame;
              entry = encodeEntry(newFrame);
              mem.mem_contents[entryAddr]= entry;
          }
  
          uint64_t frame;
          bool present, write, user;
          decodeEntry(entry, frame, present, write, user);
  
          if (!present) {throw runtime_error("Page not present")};
          if (!write && isWrite) {throw runtime_error("Writing not allowed in read-only page")};
  
          curr = frame;
      }
      uint64_t paddr = curr + offset;
      return paddr;
    }
};
