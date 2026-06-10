#include "FileManager.h"
#include "OrderScheduler.h"   // for computeScore
#include <fstream>
#include <sstream>
#include <iostream>

namespace Files {

// split "a,b,c" into fields
static DynamicArray<std::string> split(const std::string& s, char d = ',') {
    DynamicArray<std::string> out;
    std::string cur;
    std::stringstream ss(s);
    while (std::getline(ss, cur, d)) out.push_back(cur);
    return out;
}
static int toInt(const std::string& s) { return s.empty() ? 0 : std::atoi(s.c_str()); }

// ----------------------------- seed defaults ------------------------------
// Used only when the data files are missing, so the program always runs.
static void seedDefaults(System& sys) {
    sys.customers.clear(); sys.restaurants.clear();
    sys.riders.clear();    sys.orders.clear();

    const char* cnames[] = { "Ahmed", "Fatima", "Usman", "Zainab" };
    bool cvip[]          = { false,    true,     false,   true };
    for (int i = 0; i < 4; i++) { Customer c; c.id = i + 1; c.name = cnames[i]; c.vip = cvip[i]; sys.customers.push_back(c); }

    Restaurant r1; r1.id = 1; r1.name = "Pizza Palace"; r1.node = 0; r1.capacity = 5; sys.restaurants.push_back(r1);
    Restaurant r2; r2.id = 2; r2.name = "Burger Barn";  r2.node = 2; r2.capacity = 4; sys.restaurants.push_back(r2);
    Restaurant r3; r3.id = 3; r3.name = "Sushi Spot";   r3.node = 3; r3.capacity = 3; sys.restaurants.push_back(r3);

    Rider d1; d1.id = 1; d1.name = "Ali";   d1.node = 0; d1.capacity = 3; sys.riders.push_back(d1);
    Rider d2; d2.id = 2; d2.name = "Sara";  d2.node = 1; d2.capacity = 3; sys.riders.push_back(d2);
    Rider d3; d3.id = 3; d3.name = "Bilal"; d3.node = 4; d3.capacity = 2; sys.riders.push_back(d3);

    // city map
    sys.cityMap.init(6);
    const char* nn[] = { "Downtown", "Uptown", "Riverside", "Hilltop", "Eastend", "Westgate" };
    for (int i = 0; i < 6; i++) sys.cityMap.setName(i, nn[i]);
    sys.cityMap.addEdge(0, 1, 4); sys.cityMap.addEdge(0, 2, 3); sys.cityMap.addEdge(1, 2, 1);
    sys.cityMap.addEdge(1, 3, 2); sys.cityMap.addEdge(2, 4, 5); sys.cityMap.addEdge(3, 4, 3);
    sys.cityMap.addEdge(3, 5, 6); sys.cityMap.addEdge(4, 5, 2); sys.cityMap.addEdge(0, 5, 8);

    // a few orders across the lifecycle
    struct Seed { int cust, rest, prio, prep, dl, dst, st; };
    Seed seeds[] = {
        {1, 1, 2, 12, 30, 4, 0}, // Placed
        {2, 1, 5, 10, 20, 5, 0}, // Placed (VIP, urgent)
        {3, 2, 1, 15, 40, 3, 2}, // Queued
        {4, 3, 4,  8, 18, 1, 3}, // Prepared
        {1, 2, 1, 20, 60, 4, 6}, // Delivered
    };
    for (int i = 0; i < 5; i++) {
        Order o; o.id = 1000 + i; o.customerId = seeds[i].cust; o.restaurantId = seeds[i].rest;
        o.priority = seeds[i].prio; o.prepTime = seeds[i].prep; o.deadline = seeds[i].dl;
        o.destNode = seeds[i].dst; o.state = (OrderState)seeds[i].st;
        o.srcNode = 0; // filled in below once indexes exist
        sys.orders.push_back(o);
    }
    sys.rebuildIndexes();
    for (int i = 0; i < sys.orders.size(); i++) {
        Restaurant* r = sys.findRestaurant(sys.orders[i].restaurantId);
        if (r) sys.orders[i].srcNode = r->node;
    }
}

// ------------------------------ loaders -----------------------------------
static bool loadCustomers(System& sys) {
    std::ifstream f(DIR + "customers.txt");
    if (!f) return false;
    std::string ln;
    while (std::getline(f, ln)) {
        if (ln.empty()) continue;
        DynamicArray<std::string> p = split(ln);
        if (p.size() < 3) continue;
        Customer c; c.id = toInt(p[0]); c.name = p[1]; c.vip = toInt(p[2]) != 0;
        sys.customers.push_back(c);
    }
    return true;
}
static bool loadRestaurants(System& sys) {
    std::ifstream f(DIR + "restaurants.txt");
    if (!f) return false;
    std::string ln;
    while (std::getline(f, ln)) {
        if (ln.empty()) continue;
        DynamicArray<std::string> p = split(ln);
        if (p.size() < 4) continue;
        Restaurant r; r.id = toInt(p[0]); r.name = p[1]; r.node = toInt(p[2]);
        r.capacity = toInt(p[3]); r.currentLoad = 0;
        sys.restaurants.push_back(r);
    }
    return true;
}
static bool loadRiders(System& sys) {
    std::ifstream f(DIR + "riders.txt");
    if (!f) return false;
    std::string ln;
    while (std::getline(f, ln)) {
        if (ln.empty()) continue;
        DynamicArray<std::string> p = split(ln);
        if (p.size() < 6) continue;
        Rider r; r.id = toInt(p[0]); r.name = p[1]; r.node = toInt(p[2]);
        r.load = toInt(p[3]); r.capacity = toInt(p[4]); r.available = toInt(p[5]) != 0;
        sys.riders.push_back(r);
    }
    return true;
}
static bool loadOrders(System& sys) {
    std::ifstream f(DIR + "orders.txt");
    if (!f) return false; // not fatal; caller decides
    std::string ln;
    while (std::getline(f, ln)) {
        if (ln.empty()) continue;
        DynamicArray<std::string> p = split(ln);
        if (p.size() < 10) continue;
        Order o;
        o.id = toInt(p[0]); o.customerId = toInt(p[1]); o.restaurantId = toInt(p[2]);
        o.riderId = toInt(p[3]); o.priority = toInt(p[4]); o.prepTime = toInt(p[5]);
        o.deadline = toInt(p[6]); o.srcNode = toInt(p[7]); o.destNode = toInt(p[8]);
        o.state = (OrderState)toInt(p[9]);
        sys.orders.push_back(o);
    }
    return true;
}
static bool loadCityMap(System& sys) {
    std::ifstream f(DIR + "citymap.txt");
    if (!f) return false;
    std::string ln;
    if (!std::getline(f, ln)) return false;
    int n = toInt(ln);
    sys.cityMap.init(n);
    for (int i = 0; i < n; i++) {
        if (!std::getline(f, ln)) break;
        DynamicArray<std::string> p = split(ln);
        if (p.size() >= 2) sys.cityMap.setName(toInt(p[0]), p[1]);
    }
    // remaining lines are edges (skip a header line like "EDGES" if present)
    while (std::getline(f, ln)) {
        if (ln.empty()) continue;
        DynamicArray<std::string> p = split(ln);
        if (p.size() < 3) continue;       // header or junk
        sys.cityMap.addEdge(toInt(p[0]), toInt(p[1]), toInt(p[2]));
    }
    return true;
}
static void loadHistory(System& sys) {
    std::ifstream f(DIR + "history.txt");
    if (!f) return;
    std::string ln;
    while (std::getline(f, ln)) {
        if (ln.empty()) continue;
        DynamicArray<std::string> p = split(ln);
        if (p.size() < 4) continue;
        HistoryEvent e;
        e.orderId = toInt(p[0]); e.from = (OrderState)toInt(p[1]);
        e.to = (OrderState)toInt(p[2]); e.seq = toInt(p[3]);
        sys.history.push_back(e);
        if (e.seq > sys.seqCounter) sys.seqCounter = e.seq;
    }
}

// ------------------------- derived state rebuild --------------------------
void rebuildDerived(System& sys) {
    sys.rebuildIndexes();

    // kitchen queues sized to restaurant count
    sys.kitchenQueues.clear();
    for (int i = 0; i < sys.restaurants.size(); i++)
        sys.kitchenQueues.push_back(LinkedList<int>());
    for (int i = 0; i < sys.restaurants.size(); i++) sys.restaurants[i].currentLoad = 0;

    sys.scheduler.clear();

    int maxId = 999;
    for (int i = 0; i < sys.orders.size(); i++) {
        Order& o = sys.orders[i];
        if (o.id > maxId) maxId = o.id;
        if (o.state == OrderState::Placed) {
            sys.scheduler.push(Scheduler::computeScore(o), o.id);
        } else if (o.state == OrderState::Queued) {
            int ri = sys.restaurantIndex(o.restaurantId);
            if (ri >= 0) { sys.kitchenQueues[ri].push_back(o.id); sys.restaurants[ri].currentLoad++; }
        } else if (o.state == OrderState::Prepared) {
            int ri = sys.restaurantIndex(o.restaurantId);
            if (ri >= 0) sys.restaurants[ri].currentLoad++;
        }
    }
    sys.nextOrderId = maxId + 1;
}

// --------------------------------- API ------------------------------------
bool loadAll(System& sys) {
    bool ok = loadCustomers(sys);
    ok = loadRestaurants(sys) && ok;
    ok = loadRiders(sys) && ok;
    ok = loadCityMap(sys) && ok;
    if (!ok) {
        std::cout << "[FileManager] data files missing/incomplete - seeding defaults.\n";
        // reset anything partially loaded
        sys.customers.clear(); sys.restaurants.clear(); sys.riders.clear(); sys.orders.clear();
        seedDefaults(sys);
        rebuildDerived(sys);
        return false;
    }
    loadOrders(sys);
    loadHistory(sys);
    rebuildDerived(sys);
    return true;
}

void saveAll(System& sys) {
    {
        std::ofstream f(DIR + "customers.txt");
        for (int i = 0; i < sys.customers.size(); i++)
            f << sys.customers[i].id << "," << sys.customers[i].name << ","
              << (sys.customers[i].vip ? 1 : 0) << "\n";
    }
    {
        std::ofstream f(DIR + "restaurants.txt");
        for (int i = 0; i < sys.restaurants.size(); i++)
            f << sys.restaurants[i].id << "," << sys.restaurants[i].name << ","
              << sys.restaurants[i].node << "," << sys.restaurants[i].capacity << ","
              << sys.restaurants[i].currentLoad << "\n";
    }
    {
        std::ofstream f(DIR + "riders.txt");
        for (int i = 0; i < sys.riders.size(); i++)
            f << sys.riders[i].id << "," << sys.riders[i].name << ","
              << sys.riders[i].node << "," << sys.riders[i].load << ","
              << sys.riders[i].capacity << "," << (sys.riders[i].available ? 1 : 0) << "\n";
    }
    {
        std::ofstream f(DIR + "orders.txt");
        for (int i = 0; i < sys.orders.size(); i++) {
            Order& o = sys.orders[i];
            f << o.id << "," << o.customerId << "," << o.restaurantId << "," << o.riderId
              << "," << o.priority << "," << o.prepTime << "," << o.deadline << ","
              << o.srcNode << "," << o.destNode << "," << (int)o.state << "\n";
        }
    }
    {
        std::ofstream f(DIR + "history.txt");
        sys.history.forEach([&](const HistoryEvent& e) {
            f << e.orderId << "," << (int)e.from << "," << (int)e.to << "," << e.seq << "\n";
        });
    }
    // citymap.txt is treated as read-only configuration; we do not overwrite it.
    std::cout << "[FileManager] state saved to " << DIR << "\n";
}

} // namespace Files
