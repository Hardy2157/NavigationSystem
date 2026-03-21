#include "core/traffic/TrafficModel.h"
#include <cmath>

namespace nav {

double TrafficModel::calculateTravelTime(double length, double capacity, double currentCount) {
    // Avoid division by zero
    if (capacity <= 0.0) {
        return length * C;  // Default to free flow if capacity is invalid
    }

    double ratio = currentCount / capacity;  // x = n/v

    if (ratio <= K) {
        // Free flow: T = C * L * 1.0
        return C * length * 1.0;
    } else {
        // Congested: T = C * L * (1.0 + exp(x))
        return C * length * (1.0 + std::exp(ratio));
    }
}

double TrafficModel::getCongestionRatio(double capacity, double currentCount) {
    if (capacity <= 0.0) {
        return 0.0;
    }
    return currentCount / capacity;
}

int TrafficModel::getCongestionStatus(double capacity, double currentCount) {
    double ratio = getCongestionRatio(capacity, currentCount);

    if (ratio < VISUAL_YELLOW_RATIO) {
        return 0;  // Green - low load
    } else if (ratio < VISUAL_RED_RATIO) {
        return 1;  // Yellow - moderate congestion
    } else {
        return 2;  // Red - heavy congestion
    }
}

} // namespace nav
