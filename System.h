#pragma once
// ---------------------------------------------------------------------------
// System.h - the central in-memory state of the whole engine. Every module
// receives a reference to a single System object and operates on it. This is
// the "integration" backbone that ties the modules together.
// ---------------------------------------------------------------------------
#include "Models.h"
#include "DataStructures.h"

struct System {
    // Master datasets (custom DynamicArray, not std::vector).
    DynamicArray<Customer>   customers;
    DynamicArray<Restaurant> restaurants;
    DynamicArray<Rider>      riders;
    DynamicArray<Order>      orders;        // every order ever created

    // id -> index lookup tables (custom hash table) for O(1) retrieval.
    HashIntMap<int> orderIndex;
    HashIntMap<int> custIndex;
    HashIntMap<int> restIndex;
    HashIntMap<int> riderIndex;

    // Scheduling: heap of Placed orders, keyed by urgency score.
    MinHeap scheduler;

    // Kitchen queues: one linked list of order-ids per restaurant (by index).
    DynamicArray<LinkedList<int> > kitchenQueues;

    // City map for routing.
    Graph cityMap;

    // History/tracking: append-only timeline + LIFO undo stack.
    LinkedList<HistoryEvent> history;
    Stack<HistoryEvent>      undoStack;

    int nextOrderId;   // auto-increment id generator
    int seqCounter;    // history sequence numbers

    System() : nextOrderId(1000), seqCounter(0) {}

    // ---- fast lookups (return nullptr if missing) -------------------------
    // NOTE: pointers are valid until the underlying array reallocates; use
    // them immediately and re-fetch after adding new records.
    Order*      findOrder(int id)      { int i; return orderIndex.get(id, i) ? &orders[i]      : nullptr; }
    Customer*   findCustomer(int id)   { int i; return custIndex.get(id, i)  ? &customers[i]   : nullptr; }
    Restaurant* findRestaurant(int id) { int i; return restIndex.get(id, i)  ? &restaurants[i] : nullptr; }
    Rider*      findRider(int id)      { int i; return riderIndex.get(id, i)  ? &riders[i]      : nullptr; }

    int restaurantIndex(int id) { int i; return restIndex.get(id, i) ? i : -1; }

    // Rebuild all id->index hash tables after bulk loading/mutating arrays.
    void rebuildIndexes() {
        orderIndex.clearAll();
        for (int i = 0; i < orders.size(); i++) orderIndex.put(orders[i].id, i);
        custIndex.clearAll();
        for (int i = 0; i < customers.size(); i++) custIndex.put(customers[i].id, i);
        restIndex.clearAll();
        for (int i = 0; i < restaurants.size(); i++) restIndex.put(restaurants[i].id, i);
        riderIndex.clearAll();
        for (int i = 0; i < riders.size(); i++) riderIndex.put(riders[i].id, i);
    }
};
