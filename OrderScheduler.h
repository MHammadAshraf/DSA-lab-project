#pragma once
// Module 1: Dynamic Order Scheduling.
// New orders go into a MinHeap keyed by an urgency score. "Process next"
// pops the most urgent order and hands it to the kitchen.
#include "System.h"

namespace Scheduler {
    // Lower score = handled sooner. Tight deadline and high priority both
    // lower the score.  score = deadline*10 - priority*50
    long long computeScore(const Order& o);

    void addOrder(System& sys);              // interactive create
    int  addOrderObj(System& sys, Order o);  // programmatic create -> returns id
    void cancelOrder(System& sys);
    void updatePriority(System& sys);
    void processNext(System& sys);           // pop most-urgent, send to kitchen
    void listPending(System& sys);
    void menu(System& sys);
}
