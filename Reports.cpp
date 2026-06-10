#include "Reports.h"
#include "Util.h"
#include <iostream>

namespace Reports {

void show(System& sys) {
    std::cout << "\n";
    line('=');
    std::cout << "                 FOODEXPRESS SYSTEM REPORT\n";
    line('=');

    std::cout << "Customers: " << sys.customers.size()
              << "   Restaurants: " << sys.restaurants.size()
              << "   Riders: " << sys.riders.size()
              << "   Orders: " << sys.orders.size() << "\n\n";

    // orders by state
    int counts[10] = {0};
    for (int i = 0; i < sys.orders.size(); i++) counts[(int)sys.orders[i].state]++;
    std::cout << "Orders by state:\n";
    for (int s = 0; s < 10; s++)
        if (counts[s]) std::cout << "  " << stateName((OrderState)s) << ": " << counts[s] << "\n";

    // busiest restaurant by total orders
    int bestRest = -1, bestCount = -1;
    for (int i = 0; i < sys.restaurants.size(); i++) {
        int c = 0;
        for (int j = 0; j < sys.orders.size(); j++)
            if (sys.orders[j].restaurantId == sys.restaurants[i].id) c++;
        if (c > bestCount) { bestCount = c; bestRest = i; }
    }
    if (bestRest >= 0)
        std::cout << "\nBusiest restaurant: " << sys.restaurants[bestRest].name
                  << " (" << bestCount << " orders, current load "
                  << sys.restaurants[bestRest].currentLoad << "/"
                  << sys.restaurants[bestRest].capacity << ")\n";

    // rider utilization
    std::cout << "\nRider utilization:\n";
    for (int i = 0; i < sys.riders.size(); i++) {
        Rider& r = sys.riders[i];
        int pct = r.capacity ? (r.load * 100 / r.capacity) : 0;
        std::cout << "  " << r.name << ": " << r.load << "/" << r.capacity
                  << " (" << pct << "%)\n";
    }

    std::cout << "\nDelivered: " << counts[(int)OrderState::Delivered]
              << "   Cancelled: " << counts[(int)OrderState::Cancelled] << "\n";
    line('=');
}

} // namespace Reports
