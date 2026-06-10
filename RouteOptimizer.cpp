#include "RouteOptimizer.h"
#include "Util.h"
#include <iostream>

namespace Route {

void printPath(System& sys, int src, int dst) {
    Graph& g = sys.cityMap;
    DynamicArray<int> path = g.shortestPath(src, dst);
    if (path.empty()) { std::cout << "  No route between " << src << " and " << dst << ".\n"; return; }
    long long d = g.shortestDist(src, dst);
    std::cout << "  Route: ";
    for (int i = 0; i < path.size(); i++) {
        std::cout << g.name(path[i]) << "(" << path[i] << ")";
        if (i + 1 < path.size()) std::cout << " -> ";
    }
    std::cout << "\n  Distance: " << d << " units\n";
}

void shortestRoute(System& sys) {
    int s = readInt("From node: ");
    int d = readInt("To node: ");
    printPath(sys, s, d);
}

void compareRoutes(System& sys) {
    int s  = readInt("From node: ");
    int d1 = readInt("Destination A node: ");
    int d2 = readInt("Destination B node: ");
    long long a = sys.cityMap.shortestDist(s, d1);
    long long b = sys.cityMap.shortestDist(s, d2);
    std::cout << "  A (" << s << "->" << d1 << "): "
              << (a >= INF_DIST ? -1 : a) << " units\n";
    std::cout << "  B (" << s << "->" << d2 << "): "
              << (b >= INF_DIST ? -1 : b) << " units\n";
    if (a < INF_DIST || b < INF_DIST)
        std::cout << "  Closer destination: " << (a <= b ? "A" : "B") << "\n";
}

void blockAndReroute(System& sys) {
    int s = readInt("Trip start node: ");
    int d = readInt("Trip end node: ");
    std::cout << "Original ";
    printPath(sys, s, d);

    int u = readInt("Block road - endpoint u: ");
    int v = readInt("Block road - endpoint v: ");
    if (!sys.cityMap.setBlocked(u, v, true)) {
        std::cout << "  No such road to block.\n"; return;
    }
    std::cout << "Road " << u << "-" << v << " blocked. Rerouting...\n";
    printPath(sys, s, d);

    // restore so the demo map stays intact
    sys.cityMap.setBlocked(u, v, false);
    std::cout << "(road re-opened after demo)\n";
}

void costEstimate(System& sys) {
    int s = readInt("From node: ");
    int d = readInt("To node: ");
    long long dist = sys.cityMap.shortestDist(s, d);
    if (dist >= INF_DIST) { std::cout << "  Unreachable.\n"; return; }
    std::cout << "  Distance " << dist << " units  ->  estimated cost "
              << (dist * COST_PER_UNIT) << "\n";
}

void menu(System& sys) {
    while (true) {
        std::cout << "\n--- Route Optimization ---\n";
        std::cout << "City nodes: ";
        for (int i = 0; i < sys.cityMap.nodes(); i++)
            std::cout << i << "=" << sys.cityMap.name(i) << "  ";
        std::cout << "\n"
                  << " 1. Shortest route (with path)\n"
                  << " 2. Compare two routes\n"
                  << " 3. Block a road and reroute\n"
                  << " 4. Delivery cost estimate\n"
                  << " 0. Back\n";
        int c = readInt("Choice: ");
        if (c == 0) return;
        if (c == 1) { shortestRoute(sys); pause(); }
        else if (c == 2) { compareRoutes(sys); pause(); }
        else if (c == 3) { blockAndReroute(sys); pause(); }
        else if (c == 4) { costEstimate(sys); pause(); }
    }
}

} // namespace Route
