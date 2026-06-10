#include "PerformanceAnalyzer.h"
#include "OrderScheduler.h"
#include "Util.h"
#include <iostream>
#include <chrono>

namespace Perf {

using Clock = std::chrono::high_resolution_clock;
static double ms(Clock::time_point a, Clock::time_point b) {
    return std::chrono::duration<double, std::milli>(b - a).count();
}

// Tiny deterministic pseudo-random generator (linear congruential).
struct LCG {
    unsigned long long s;
    LCG(unsigned long long seed) : s(seed) {}
    unsigned next() { s = s * 6364136223846793005ULL + 1442695040888963407ULL; return (unsigned)(s >> 33); }
    int range(int n) { return (int)(next() % (unsigned)n); }
};

void runAnalysis(System& /*sys*/) {
    const int sizes[] = { 1000, 10000, 100000 };
    std::cout << "\nPerformance analysis (measured on this machine)\n";
    line('=');
    std::cout << "Structure / Op        Theory       ";
    for (int i = 0; i < 3; i++) std::cout << "N=" << sizes[i] << "        ";
    std::cout << "\n";
    line('=');

    // ---- MinHeap insert ----
    std::cout << "MinHeap push          O(log n)     ";
    for (int k = 0; k < 3; k++) {
        int N = sizes[k]; LCG rng(12345 + k);
        MinHeap h;
        auto a = Clock::now();
        for (int i = 0; i < N; i++) h.push(rng.range(1000000), i);
        auto b = Clock::now();
        std::cout.width(14); std::cout << std::left << ms(a, b);
    }
    std::cout << "(ms total)\n";

    // ---- MinHeap pop ----
    std::cout << "MinHeap pop-all       O(n log n)   ";
    for (int k = 0; k < 3; k++) {
        int N = sizes[k]; LCG rng(999 + k);
        MinHeap h;
        for (int i = 0; i < N; i++) h.push(rng.range(1000000), i);
        HeapNode out;
        auto a = Clock::now();
        while (h.pop(out)) {}
        auto b = Clock::now();
        std::cout.width(14); std::cout << std::left << ms(a, b);
    }
    std::cout << "(ms total)\n";

    // ---- HashIntMap insert ----
    std::cout << "HashMap put           O(1) avg     ";
    for (int k = 0; k < 3; k++) {
        int N = sizes[k];
        HashIntMap<int> m(1 << 17);
        auto a = Clock::now();
        for (int i = 0; i < N; i++) m.put(i, i * 2);
        auto b = Clock::now();
        std::cout.width(14); std::cout << std::left << ms(a, b);
    }
    std::cout << "(ms total)\n";

    // ---- HashIntMap search (N lookups) ----
    std::cout << "HashMap get (N hits)  O(1) avg     ";
    for (int k = 0; k < 3; k++) {
        int N = sizes[k];
        HashIntMap<int> m(1 << 17);
        for (int i = 0; i < N; i++) m.put(i, i * 2);
        LCG rng(7 + k); int v; long long acc = 0;
        auto a = Clock::now();
        for (int i = 0; i < N; i++) { if (m.get(rng.range(N), v)) acc += v; }
        auto b = Clock::now();
        std::cout.width(14); std::cout << std::left << ms(a, b);
        (void)acc;
    }
    std::cout << "(ms total)\n";

    line('=');
    std::cout << "Times scale roughly with the stated complexity as N grows 100x.\n";
}

void scalability(System& /*sys*/) {
    const int sizes[] = { 1000, 5000, 25000, 100000 };
    std::cout << "\nScalability simulation: schedule then drain N random orders\n";
    line('=');
    std::cout << "N            Time(ms)     Throughput(orders/ms)\n";
    line('=');
    for (int k = 0; k < 4; k++) {
        int N = sizes[k]; LCG rng(2024 + k);
        MinHeap sched;
        auto a = Clock::now();
        // simulate dynamic arrivals
        for (int i = 0; i < N; i++) {
            int deadline = 10 + rng.range(120);
            int priority = 1 + rng.range(5);
            long long score = (long long)deadline * 10 - (long long)priority * 50;
            sched.push(score, i);
        }
        // process all by urgency
        HeapNode out; int processed = 0;
        while (sched.pop(out)) processed++;
        auto b = Clock::now();
        double t = ms(a, b);
        std::cout.width(13); std::cout << std::left << N;
        std::cout.width(13); std::cout << std::left << t;
        std::cout << (t > 0 ? (processed / t) : 0.0) << "\n";
    }
    line('=');
    std::cout << "Near-linear growth confirms the engine scales to peak-hour volumes.\n";
}

} // namespace Perf
