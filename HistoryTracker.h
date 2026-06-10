#pragma once
// Module 6: Order History & Tracking.
// Records every state transition in an append-only timeline (LinkedList) and
// supports undo of the most recent transition (Stack) plus timeline replay.
#include "System.h"

namespace History {
    // Apply a state change to an order AND record it. All other modules call
    // this instead of writing order.state directly, so the audit log is complete.
    void transition(System& sys, Order& o, OrderState ns);

    void replay(System& sys, int orderId);   // print one order's timeline
    bool undoLast(System& sys);              // revert the most recent transition
    void menu(System& sys);
}
