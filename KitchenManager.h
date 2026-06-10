#pragma once
// Module 2: Kitchen Load Balancing.
// Each restaurant owns a LinkedList<int> queue of order-ids (in System).
// Detects overloaded kitchens, estimates wait times, and rebalances load.
#include "System.h"

namespace Kitchen {
    const int AVG_PREP = 8; // minutes per order, used for wait estimation

    void enqueue(System& sys, Order& o);   // place a Queued order into its kitchen
    void viewLoads(System& sys);
    void prepareNext(System& sys);         // finish-cooking the front order of a kitchen
    void rebalance(System& sys);           // move work off overloaded kitchens
    void menu(System& sys);
}
