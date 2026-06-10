#include "SearchEngine.h"
#include "Util.h"
#include <iostream>

namespace Search {

void printOrder(System& sys, const Order& o) {
    Customer*   c = sys.findCustomer(o.customerId);
    Restaurant* r = sys.findRestaurant(o.restaurantId);
    std::cout << "  Order " << o.id
              << " | cust " << o.customerId << (c ? " (" + c->name + ")" : "")
              << " | rest " << o.restaurantId << (r ? " (" + r->name + ")" : "")
              << " | rider " << o.riderId
              << " | prio " << o.priority
              << " | " << stateName(o.state) << "\n";
}

void findById(System& sys) {
    int c = readInt("Find: 1=order 2=customer 3=restaurant 4=rider : ");
    int id = readInt("id: ");
    if (c == 1) { Order* o = sys.findOrder(id);      if (o) printOrder(sys, *o); else std::cout << "  not found\n"; }
    else if (c == 2) { Customer* x = sys.findCustomer(id);   if (x) std::cout << "  Customer " << x->id << " " << x->name << (x->vip ? " [VIP]" : "") << "\n"; else std::cout << "  not found\n"; }
    else if (c == 3) { Restaurant* x = sys.findRestaurant(id); if (x) std::cout << "  Restaurant " << x->id << " " << x->name << " node " << x->node << " load " << x->currentLoad << "/" << x->capacity << "\n"; else std::cout << "  not found\n"; }
    else if (c == 4) { Rider* x = sys.findRider(id);     if (x) std::cout << "  Rider " << x->id << " " << x->name << " node " << x->node << " load " << x->load << "/" << x->capacity << "\n"; else std::cout << "  not found\n"; }
}

void filterByState(System& sys) {
    std::cout << "States: 0=Placed 1=Accepted 2=Queued 3=Prepared 4=Assigned "
                 "5=PickedUp 6=Delivered 7=Delayed 8=Cancelled 9=Rerouted\n";
    int s = readInt("State number: ");
    int n = 0;
    for (int i = 0; i < sys.orders.size(); i++)
        if ((int)sys.orders[i].state == s) { printOrder(sys, sys.orders[i]); n++; }
    std::cout << "  " << n << " match(es).\n";
}

void filterByRestaurant(System& sys) {
    int rid = readInt("Restaurant id: ");
    int n = 0;
    for (int i = 0; i < sys.orders.size(); i++)
        if (sys.orders[i].restaurantId == rid) { printOrder(sys, sys.orders[i]); n++; }
    std::cout << "  " << n << " order(s) for restaurant " << rid << ".\n";
}

void vipOrders(System& sys) {
    int n = 0;
    for (int i = 0; i < sys.orders.size(); i++) {
        Customer* c = sys.findCustomer(sys.orders[i].customerId);
        if (c && c->vip) { printOrder(sys, sys.orders[i]); n++; }
    }
    std::cout << "  " << n << " VIP order(s).\n";
}

void menu(System& sys) {
    while (true) {
        std::cout << "\n--- Search & Retrieval Engine ---\n"
                  << " 1. Find record by id (hash lookup, O(1) avg)\n"
                  << " 2. Filter orders by state\n"
                  << " 3. Filter orders by restaurant\n"
                  << " 4. VIP customers' orders\n"
                  << " 0. Back\n";
        int c = readInt("Choice: ");
        if (c == 0) return;
        if (c == 1) { findById(sys); pause(); }
        else if (c == 2) { filterByState(sys); pause(); }
        else if (c == 3) { filterByRestaurant(sys); pause(); }
        else if (c == 4) { vipOrders(sys); pause(); }
    }
}

} // namespace Search
