#include "KitchenManager.h"
#include "HistoryTracker.h"
#include "Util.h"
#include <iostream>

namespace Kitchen {

void enqueue(System& sys, Order& o) {
    int ri = sys.restaurantIndex(o.restaurantId);
    if (ri < 0) { std::cout << "  Unknown restaurant for order " << o.id << "\n"; return; }
    sys.kitchenQueues[ri].push_back(o.id);
    sys.restaurants[ri].currentLoad++;
}

void viewLoads(System& sys) {
    std::cout << "\nKitchen load report\n";
    line();
    std::cout << "ID   Name                 Load/Cap   Wait(min)   Status\n";
    line();
    for (int i = 0; i < sys.restaurants.size(); i++) {
        Restaurant& r = sys.restaurants[i];
        int wait = sys.kitchenQueues[i].size() * AVG_PREP;
        bool over = r.currentLoad > r.capacity;
        std::cout.width(4);  std::cout << std::left << r.id;
        std::cout.width(21); std::cout << std::left << r.name;
        std::cout << r.currentLoad << "/" << r.capacity << "        ";
        std::cout.width(11); std::cout << std::left << wait;
        std::cout << (over ? "OVERLOADED" : "ok") << "\n";
    }
    line();
}

void prepareNext(System& sys) {
    int rid = readInt("Restaurant id to prepare next order at: ");
    int ri = sys.restaurantIndex(rid);
    if (ri < 0) { std::cout << "No such restaurant.\n"; return; }
    int orderId;
    if (!sys.kitchenQueues[ri].pop_front(orderId)) {
        std::cout << "Kitchen queue is empty.\n"; return;
    }
    Order* o = sys.findOrder(orderId);
    if (o) {
        History::transition(sys, *o, OrderState::Prepared);
        std::cout << "Order " << orderId << " is now Prepared and ready for a rider.\n";
    }
    // load stays counted until the order is picked up by a rider
}

void rebalance(System& sys) {
    std::cout << "\nRebalancing overloaded kitchens...\n";
    bool any = false;
    for (int i = 0; i < sys.restaurants.size(); i++) {
        Restaurant& r = sys.restaurants[i];
        while (r.currentLoad > r.capacity && !sys.kitchenQueues[i].empty()) {
            // find a target restaurant with spare capacity
            int target = -1;
            for (int j = 0; j < sys.restaurants.size(); j++)
                if (j != i && sys.restaurants[j].currentLoad < sys.restaurants[j].capacity) {
                    target = j; break;
                }
            if (target < 0) { std::cout << "  No spare capacity anywhere to offload "
                                        << r.name << ".\n"; break; }
            int movedId = 0;
            sys.kitchenQueues[i].pop_front(movedId);
            r.currentLoad--;
            Restaurant& t = sys.restaurants[target];
            sys.kitchenQueues[target].push_back(movedId);
            t.currentLoad++;
            Order* o = sys.findOrder(movedId);
            if (o) { o->restaurantId = t.id; o->srcNode = t.node;
                     History::transition(sys, *o, OrderState::Queued); }
            std::cout << "  Moved order " << movedId << " from " << r.name
                      << " to " << t.name << "\n";
            any = true;
        }
    }
    if (!any) std::cout << "  All kitchens within capacity. Nothing to do.\n";
}

void menu(System& sys) {
    while (true) {
        std::cout << "\n--- Kitchen Load Analysis ---\n"
                  << " 1. View kitchen loads & wait times\n"
                  << " 2. Prepare next order at a kitchen\n"
                  << " 3. Rebalance overloaded kitchens\n"
                  << " 0. Back\n";
        int c = readInt("Choice: ");
        if (c == 0) return;
        if (c == 1) { viewLoads(sys); pause(); }
        else if (c == 2) { prepareNext(sys); pause(); }
        else if (c == 3) { rebalance(sys); pause(); }
    }
}

} // namespace Kitchen
