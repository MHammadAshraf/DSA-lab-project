// ===========================================================================
//  FoodExpress Dispatch Optimization Engine
//  CSC-200L Data Structures & Algorithms - Open Ended Lab
//
//  CLI/menu-based engine built on custom data structures (no STL containers
//  for the major structures). Each numbered menu item maps to one module.
// ===========================================================================
#include "System.h"
#include "FileManager.h"
#include "OrderScheduler.h"
#include "KitchenManager.h"
#include "RiderDispatch.h"
#include "RouteOptimizer.h"
#include "SearchEngine.h"
#include "HistoryTracker.h"
#include "PerformanceAnalyzer.h"
#include "Reports.h"
#include "Util.h"
#include <iostream>

static void banner() {
    line('=');
    std::cout << "        FoodExpress Dispatch Optimization Engine\n";
    line('=');
}

int main() {
    System sys;
    Files::loadAll(sys);

    banner();
    std::cout << "Loaded " << sys.orders.size() << " orders, "
              << sys.restaurants.size() << " restaurants, "
              << sys.riders.size() << " riders.\n";

    while (true) {
        std::cout << "\n";
        line();
        std::cout << " 1. Dynamic Order Scheduling\n"
                  << " 2. Kitchen Load Analysis\n"
                  << " 3. Rider Dispatch Optimization\n"
                  << " 4. Route Optimization\n"
                  << " 5. Search and Retrieval Engine\n"
                  << " 6. Order History Tracking\n"
                  << " 7. Performance Analysis\n"
                  << " 8. Scalability Simulation\n"
                  << " 9. System Reports\n"
                  << "10. Exit (save & quit)\n";
        line();
        int c = readInt("Select: ");

        switch (c) {
            case 1: Scheduler::menu(sys); break;
            case 2: Kitchen::menu(sys);   break;
            case 3: Dispatch::menu(sys);  break;
            case 4: Route::menu(sys);     break;
            case 5: Search::menu(sys);    break;
            case 6: History::menu(sys);   break;
            case 7: Perf::runAnalysis(sys); pause(); break;
            case 8: Perf::scalability(sys); pause(); break;
            case 9: Reports::show(sys);     pause(); break;
            case 10:
                Files::saveAll(sys);
                std::cout << "Goodbye.\n";
                return 0;
            default:
                std::cout << "Unknown option.\n";
        }
    }
}
