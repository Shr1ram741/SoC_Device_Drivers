#include <iostream>
#include "MMU.hpp"
#include "Memory.hpp"
#include "Config.h"

using namespace std;

int main() {
    try {
        Config config;
        config.addressWidth = 32;
        config.pageSize = 4096;
        config.pageTableLevels = 1;
        config.bitsPerLevel = {20};

        Memory mem(config);
        MMU mmu(mem, config);

        uint64_t vaddr1 = 0x12345000;
        uint64_t vaddr2 = 0x12345001;

        mmu.storeVal(vaddr1, 23);
        mmu.storeVal(vaddr2, 729);

        cout << mmu.loadVal(vaddr1) << endl;
        cout << mmu.loadVal(vaddr2) << endl;

        mmu.loadVal(0x00000000);

    } catch (runtime_error& e) {
        cerr << "Exception: " << e.what() << endl;
    }
}
