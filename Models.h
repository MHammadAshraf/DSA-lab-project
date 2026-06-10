#pragma once
// ---------------------------------------------------------------------------
// Models.h - plain data records used throughout the engine + the order state
// machine. No logic lives here beyond tiny helpers.
// ---------------------------------------------------------------------------
#include <string>

// The full lifecycle a food order can move through.
enum class OrderState {
    Placed,     // 0  customer just placed it (sits in the scheduler heap)
    Accepted,   // 1  accepted by platform
    Queued,     // 2  sitting in a restaurant kitchen queue
    Prepared,   // 3  cooking finished, ready for a rider
    Assigned,   // 4  a rider has been dispatched
    PickedUp,   // 5  rider collected the food
    Delivered,  // 6  done
    Delayed,    // 7  flagged late
    Cancelled,  // 8  cancelled by customer/system
    Rerouted    // 9  route changed (e.g. road blocked)
};

inline std::string stateName(OrderState s) {
    switch (s) {
        case OrderState::Placed:    return "Placed";
        case OrderState::Accepted:  return "Accepted";
        case OrderState::Queued:    return "Queued";
        case OrderState::Prepared:  return "Prepared";
        case OrderState::Assigned:  return "Assigned";
        case OrderState::PickedUp:  return "PickedUp";
        case OrderState::Delivered: return "Delivered";
        case OrderState::Delayed:   return "Delayed";
        case OrderState::Cancelled: return "Cancelled";
        case OrderState::Rerouted:  return "Rerouted";
    }
    return "Unknown";
}

// An order is "pending scheduling" only while it is still Placed.
inline bool isPending(OrderState s) { return s == OrderState::Placed; }

struct Order {
    int id;
    int customerId;
    int restaurantId;
    int riderId;       // -1 until a rider is assigned
    int priority;      // higher = more urgent (VIP / express)
    int prepTime;      // minutes to cook
    int deadline;      // minutes from now the customer expects delivery
    int srcNode;       // restaurant location on the city map
    int destNode;      // customer location on the city map
    OrderState state;
    Order() : id(0), customerId(0), restaurantId(0), riderId(-1), priority(1),
              prepTime(10), deadline(30), srcNode(0), destNode(0),
              state(OrderState::Placed) {}
};

struct Customer {
    int id;
    std::string name;
    bool vip;
    Customer() : id(0), vip(false) {}
};

struct Restaurant {
    int id;
    std::string name;
    int node;          // location on the city map
    int capacity;      // max simultaneous orders it can handle
    int currentLoad;   // orders currently Queued or Prepared here
    Restaurant() : id(0), node(0), capacity(5), currentLoad(0) {}
};

struct Rider {
    int id;
    std::string name;
    int node;          // current location on the city map
    int load;          // active deliveries in hand
    int capacity;      // max deliveries the rider can carry
    bool available;
    Rider() : id(0), node(0), load(0), capacity(3), available(true) {}
};

// One immutable record of a state change, used by the history/tracking module.
struct HistoryEvent {
    int orderId;
    OrderState from;
    OrderState to;
    int seq;           // monotonically increasing event number
    HistoryEvent() : orderId(0), from(OrderState::Placed),
                     to(OrderState::Placed), seq(0) {}
};
