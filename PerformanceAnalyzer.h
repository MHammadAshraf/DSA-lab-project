#pragma once
// Module 7 + 8: Performance Analysis and Scalability Simulation.
// Empirically times our own MinHeap and HashIntMap across growing dataset
// sizes and prints results next to the theoretical complexity.
#include "System.h"

namespace Perf {
    void runAnalysis(System& sys);     // menu 7: insert/search/delete timings
    void scalability(System& sys);     // menu 8: end-to-end scheduling throughput
}
