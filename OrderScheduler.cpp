#include "OrderScheduler.h"
#include "KitchenManager.h"
#include "HistoryTracker.h"
#include "Util.h"
#include <iostream>

namespace Scheduler {

long long computeScore(const Order& o) {
    return (long long)o.deadline * 10 - (long long)o.priority * 50;
}

// Create an order object and register it everywhere. Returns its new id.
int addOrderObj(System& sys, Order o) {
    o.id = sys.nextOrderId++;
    o.state = OrderState::Placed;
    sys.orders.push_back(o);
    sys.orderIndex.put(o.id, sys.orders.size() - 1);
    sys.scheduler.push(computeScore(o), o.id);
    return o.id;
}

void addOrder(System& sys) {
    Order o;
    o.customerId   = readInt("Customer id: ");
    o.restaurantId = readInt("Restaurant id: ");
    o.priority     = readInt("Priority (1 normal .. 5 VIP/express): ");
    o.prepTime     = readInt("Prep time (min): ");
    o.deadline     = readInt("Deadline (min from now): ");
    o.destNode     = readInt("Customer location node: ");

    Restaurant* r = sys.findRestaurant(o.restaurantId);
    if (!r) { std::cout << "No such restaurant - order not created.\n"; return; }
    o.srcNode = r->node;

    int id = addOrderObj(sys, o);
    std::cout << "Created order " << id << " (state Placed, score "
              << computeScore(o) << ").\n";
}

void cancelOrder(System& sys) {
    int id = readInt("Order id to cancel: ");
    Order* o = sys.findOrder(id);
    if (!o) { std::cout << "No such order.\n"; return; }
    if (o->state == OrderState::Delivered || o->state == OrderState::Cancelled) {
        std::cout << "Order is already " << stateName(o->state) << ".\n"; return;
    }
    History::transition(sys, *o, OrderState::Cancelled);
    // heap entry is left in place; it is skipped on pop (lazy deletion).
    std::cout << "Order " << id << " cancelled.\n";
}

void updatePriority(System& sys) {
    int id = readInt("Order id: ");
    Order* o = sys.findOrder(id);
    if (!o) { std::cout << "No such order.\n"; return; }
    o->priority = readInt("New priority: ");
    // push a fresh entry; the stale one is discarded on pop (score mismatch).
    if (isPending(o->state)) sys.scheduler.push(computeScore(*o), id);
    std::cout << "Priority updated; rescheduled.\n";
}

void processNext(System& sys) {
    HeapNode top;
    while (sys.scheduler.pop(top)) {
        Order* o = sys.findOrder(top.id);
        if (!o) continue;
        // skip stale entries: cancelled / already moved on / outdated score
        if (!isPending(o->state)) continue;
        if (top.score != computeScore(*o)) continue;

        std::cout << "Processing order " << o->id << " (priority " << o->priority
                  << ", deadline " << o->deadline << ").\n";
        History::transition(sys, *o, OrderState::Queued);
        Kitchen::enqueue(sys, *o);
        std::cout << "  -> queued at restaurant " << o->restaurantId << ".\n";
        return;
    }
    std::cout << "No pending orders to process.\n";
}

void listPending(System& sys) {
    std::cout << "\nPending (Placed) orders, by id:\n";
    int n = 0;
    for (int i = 0; i < sys.orders.size(); i++) {
        Order& o = sys.orders[i];
        if (isPending(o.state)) {
            std::cout << "  order " << o.id << "  cust " << o.customerId
                      << "  rest " << o.restaurantId << "  prio " << o.priority
                      << "  deadline " << o.deadline
                      << "  score " << computeScore(o) << "\n";
            n++;
        }
    }
    if (!n) std::cout << "  (none)\n";
    std::cout << "Heap size (incl. stale entries): " << sys.scheduler.size() << "\n";
}

void menu(System& sys) {
    while (true) {
        std::cout << "\n--- Dynamic Order Scheduling ---\n"
                  << " 1. Add new order\n"
                  << " 2. Cancel order\n"
                  << " 3. Update order priority\n"
                  << " 4. Process next order (pop most urgent)\n"
                  << " 5. List pending orders\n"
                  << " 0. Back\n";
        int c = readInt("Choice: ");
        if (c == 0) return;
        if (c == 1) { addOrder(sys); pause(); }
        else if (c == 2) { cancelOrder(sys); pause(); }
        else if (c == 3) { updatePriority(sys); pause(); }
        else if (c == 4) { processNext(sys); pause(); }
        else if (c == 5) { listPending(sys); pause(); }
    }
}

} // namespace Scheduler
