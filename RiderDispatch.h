#pragma once
// Module 3: Rider Dispatch & Assignment Optimization.
// Scores available riders by route distance to the pickup + current load,
// and assigns the best one. Also handles pickup / delivery transitions.
#include "System.h"

namespace Dispatch {
    void assignRider(System& sys);   // pick best rider for a Prepared order
    void markPicked(System& sys);    // Assigned -> PickedUp
    void markDelivered(System& sys); // PickedUp -> Delivered
    void riderStatus(System& sys);
    void menu(System& sys);
}
