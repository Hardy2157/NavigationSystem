#ifndef TRAFFICSIMULATOR_H
#define TRAFFICSIMULATOR_H

#include "core/graph/Graph.h"
#include "core/traffic/TrafficModel.h"
#include <random>
#include <vector>

namespace nav {

class TrafficSimulator {
public:
    explicit TrafficSimulator(Graph& graph, unsigned int seed = 42);

    // Perform one simulation step
    void step();

    // Get list of edges that changed in last step
    const std::vector<Edge::Id>& getChangedEdges() const { return changedEdges_; }

    // Get graph reference for visual updates
    Graph& getGraph() { return graph_; }

    // Reset all traffic to zero
    void reset();

private:
    Graph& graph_;
    std::mt19937 rng_;
    std::vector<Edge::Id> changedEdges_;
    std::vector<Edge::Id> allEdgeIds_;
};

} // namespace nav

#endif // TRAFFICSIMULATOR_H
