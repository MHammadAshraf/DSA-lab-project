#include "HistoryTracker.h"
#include "Util.h"
#include <iostream>

namespace History {

void transition(System& sys, Order& o, OrderState ns) {
    HistoryEvent e;
    e.orderId = o.id;
    e.from    = o.state;
    e.to      = ns;
    e.seq     = ++sys.seqCounter;
    sys.history.push_back(e);   // append to permanent timeline (audit trail)
    sys.undoStack.push(e);      // remember it so we can undo
    o.state = ns;
}

void replay(System& sys, int orderId) {
    std::cout << "\nTimeline for order " << orderId << ":\n";
    int shown = 0;
    sys.history.forEach([&](const HistoryEvent& e) {
        if (e.orderId == orderId) {
            std::cout << "  #" << e.seq << "  " << stateName(e.from)
                      << " -> " << stateName(e.to) << "\n";
            shown++;
        }
    });
    if (shown == 0) std::cout << "  (no recorded transitions)\n";
}

bool undoLast(System& sys) {
    HistoryEvent e;
    if (!sys.undoStack.pop(e)) {
        std::cout << "Nothing to undo.\n";
        return false;
    }
    Order* o = sys.findOrder(e.orderId);
    if (!o) { std::cout << "Order " << e.orderId << " no longer exists.\n"; return false; }
    o->state = e.from;   // revert live state (timeline log is kept as audit trail)
    std::cout << "Undid order " << e.orderId << ": "
              << stateName(e.to) << " -> " << stateName(e.from) << "\n";
    return true;
}

void menu(System& sys) {
    while (true) {
        std::cout << "\n--- Order History & Tracking ---\n"
                  << " 1. Replay an order's timeline\n"
                  << " 2. Undo last state change\n"
                  << " 3. Show full history log\n"
                  << " 0. Back\n";
        int c = readInt("Choice: ");
        if (c == 0) return;
        if (c == 1) {
            int id = readInt("Order id: ");
            replay(sys, id);
            pause();
        } else if (c == 2) {
            undoLast(sys);
            pause();
        } else if (c == 3) {
            std::cout << "\nFull transition log (" << "newest last):\n";
            int n = 0;
            sys.history.forEach([&](const HistoryEvent& e) {
                std::cout << "  #" << e.seq << "  order " << e.orderId << "  "
                          << stateName(e.from) << " -> " << stateName(e.to) << "\n";
                n++;
            });
            if (n == 0) std::cout << "  (empty)\n";
            pause();
        }
    }
}

} // namespace History
