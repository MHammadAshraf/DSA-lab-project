#pragma once
// Module 5: Search & Retrieval Engine.
// O(1) average id lookups via the hash tables in System, plus linear
// filtered scans (by state / restaurant / VIP) over the order dataset.
#include "System.h"

namespace Search {
    void findById(System& sys);
    void filterByState(System& sys);
    void filterByRestaurant(System& sys);
    void vipOrders(System& sys);
    void menu(System& sys);

    void printOrder(System& sys, const Order& o);
}
