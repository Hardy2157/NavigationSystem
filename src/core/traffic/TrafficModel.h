#ifndef TRAFFICMODEL_H
#define TRAFFICMODEL_H

namespace nav {

// Stateless utility class for traffic calculations
// Uses the formula: T = C * L * f(x) where x = n/v (cars/capacity)
// f(x) = 1.0 if x <= K, else 1.0 + exp(x)
class TrafficModel {
public:
    // Constants
    static constexpr double C = 1.0;  // Time constant
    static constexpr double K = 1.0;  // Threshold for congestion
    static constexpr double VISUAL_YELLOW_RATIO = 0.25;  // Show moderate load earlier in the heatmap
    static constexpr double VISUAL_RED_RATIO = 0.5;      // Heavy load for visualization

    // Calculate travel time for an edge
    // length: physical length of the edge (L)
    // capacity: maximum vehicle capacity (v)
    // currentCount: current number of cars on edge (n)
    // Returns: travel time T = C * L * f(x) where x = n/v
    static double calculateTravelTime(double length, double capacity, double currentCount);

    // Get the congestion ratio (n/v)
    static double getCongestionRatio(double capacity, double currentCount);

    // Get congestion status for visualization
    // Returns: 0 = Green (free flow), 1 = Yellow (moderate), 2 = Red (congested)
    static int getCongestionStatus(double capacity, double currentCount);
};

} // namespace nav

#endif // TRAFFICMODEL_H
