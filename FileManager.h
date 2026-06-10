#pragma once
// File handling: load the whole system from data/*.txt at startup and save it
// back on exit. Also rebuilds derived state (indexes, heap, kitchen queues)
// from the loaded records.
#include "System.h"
#include <string>

namespace Files {
    const std::string DIR = "data/";

    bool loadAll(System& sys);   // returns false if core files were missing
    void saveAll(System& sys);
    void rebuildDerived(System& sys); // indexes + scheduler heap + kitchen queues
}
