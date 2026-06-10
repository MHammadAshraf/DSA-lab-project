#include "RiderDispatch.h"
#include "HistoryTracker.h"
#include "Util.h"
#include <iostream>

namespace Dispatch {

void assignRider(System& sys) {
    int oid = readInt("Order id to dispatch (must be Prepared): ");
    Order* o = sys.findOrder(oid);
    if (!o) { std::cout << "No such order.\n"; return; }
    if (o->state != OrderState::Prepared) {
        std::cout << "Order is " << stateName(o->state)
                  << "; only Prepared orders can be dispatched.\n"; return;
    }

    // Score every available rider:  distance(rider -> pickup) + load penalty.
    // Lower score wins. Riders at/over capacity are skipped.
    int best = -1;
    long long bestScore = INF_DIST;
    for (int i = 0; i < sys.riders.size(); i++) {
        Rider& r = sys.riders[i];
        if (!r.available || r.load >= r.capacity) continue;
        long long dist = sys.cityMap.shortestDist(r.node, o->srcNode);
        if (dist >= INF_DIST) continue;          // cannot reach the restaurant
        long long score = dist + (long long)r.load * 3; // balance workload
        if (score < bestScore) { bestScore = score; best = i; }
    }

    if (best < 0) { std::cout << "No suitable rider available right now.\n"; return; }

    Rider& r = sys.riders[best];
    o->riderId = r.id;
    r.load++;
    if (r.load >= r.capacity) r.available = false;
    History::transition(sys, *o, OrderState::Assigned);
    std::cout << "Assigned rider " << r.id << " (" << r.name << ") to order "
              << oid << "  [score " << bestScore << ", rider load now "
              << r.load << "/" << r.capacity << "].\n";
}

void markPicked(System& sys) {
    int oid = readInt("Order id picked up: ");
    Order* o = sys.findOrder(oid);
    if (!o) { std::cout << "No such order.\n"; return; }
    if (o->state != OrderState::Assigned) {
        std::cout << "Order is " << stateName(o->state) << "; expected Assigned.\n"; return;
    }
    // order leaves the kitchen now -> free up restaurant load
    Restaurant* rest = sys.findRestaurant(o->restaurantId);
    if (rest && rest->currentLoad > 0) rest->currentLoad--;
    History::transition(sys, *o, OrderState::PickedUp);
    std::cout << "Order " << oid << " picked up.\n";
}

void markDelivered(System& sys) {
    int oid = readInt("Order id delivered: ");
    Order* o = sys.findOrder(oid);
    if (!o) { std::cout << "No such order.\n"; return; }
    if (o->state != OrderState::PickedUp) {
        std::cout << "Order is " << stateName(o->state) << "; expected PickedUp.\n"; return;
    }
    Rider* r = sys.findRider(o->riderId);
    if (r) {
        if (r->load > 0) r->load--;
        r->available = true;
        r->node = o->destNode;   // rider ends up at the customer location
    }
    History::transition(sys, *o, OrderState::Delivered);
    std::cout << "Order " << oid << " delivered. Rider " << o->riderId << " freed.\n";
}

void riderStatus(System& sys) {
    std::cout << "\nRider status\n";
    line();
    std::cout << "ID   Name            Node   Load/Cap   Available\n";
    line();
    for (int i = 0; i < sys.riders.size(); i++) {
        Rider& r = sys.riders[i];
        std::cout.width(4);  std::cout << std::left << r.id;
        std::cout.width(16); std::cout << std::left << r.name;
        std::cout.width(7);  std::cout << std::left << r.node;
        std::cout << r.load << "/" << r.capacity << "        "
                  << (r.available ? "yes" : "no") << "\n";
    }
    line();
}

void menu(System& sys) {
    while (true) {
        std::cout << "\n--- Rider Dispatch Optimization ---\n"
                  << " 1. Assign best rider to a Prepared order\n"
                  << " 2. Mark order picked up\n"
                  << " 3. Mark order delivered\n"
                  << " 4. View rider status\n"
                  << " 0. Back\n";
        int c = readInt("Choice: ");
        if (c == 0) return;
        if (c == 1) { assignRider(sys); pause(); }
        else if (c == 2) { markPicked(sys); pause(); }
        else if (c == 3) { markDelivered(sys); pause(); }
        else if (c == 4) { riderStatus(sys); pause(); }
    }
}

} // namespace Dispatch
