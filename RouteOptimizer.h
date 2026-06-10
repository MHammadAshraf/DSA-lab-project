#pragma once
// Module 4: Real-Time Route Optimization.
// Wraps the city Graph: shortest path, route comparison, road blocking +
// rerouting, and delivery cost estimation.
#include "System.h"

namespace Route {
    const double COST_PER_UNIT = 1.5; // currency per distance unit

    void printPath(System& sys, int src, int dst);
    void shortestRoute(System& sys);
    void compareRoutes(System& sys);
    void blockAndReroute(System& sys);
    void costEstimate(System& sys);
    void menu(System& sys);
}
