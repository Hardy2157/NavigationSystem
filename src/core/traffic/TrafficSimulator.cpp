#include "core/traffic/TrafficSimulator.h"
#include <algorithm>

namespace nav {

TrafficSimulator::TrafficSimulator(Graph& graph, unsigned int seed)
    : graph_(graph)
    , rng_(seed)
{
    // Cache all edge IDs for random selection
    for (const auto& edgePair : graph_.getEdges()) {
        allEdgeIds_.push_back(edgePair.first);
    }
}

void TrafficSimulator::step() {
    changedEdges_.clear();

    if (allEdgeIds_.empty()) {
        return;
    }

    // Randomly select up to 100 edges to modify
    size_t numEdgesToModify = std::min(size_t(100), allEdgeIds_.size());

    std::uniform_int_distribution<size_t> edgeDist(0, allEdgeIds_.size() - 1);
    std::uniform_int_distribution<int> changeDist(-2, 3);  // Slight bias toward increase

    for (size_t i = 0; i < numEdgesToModify; ++i) {
        Edge::Id edgeId = allEdgeIds_[edgeDist(rng_)];
        Edge* edge = graph_.getEdge(edgeId);

        if (edge) {
            int change = changeDist(rng_);
            int newCount = static_cast<int>(edge->getCarCount()) + change;

            // Clamp between 0 and capacity * 3
            int maxCount = static_cast<int>(edge->getCapacity() * 3.0);
            newCount = std::max(0, std::min(newCount, maxCount));

            edge->setCarCount(static_cast<uint32_t>(newCount));
            changedEdges_.push_back(edgeId);
        }
    }
}

void TrafficSimulator::reset() {
    changedEdges_.clear();

    for (const auto& edgePair : graph_.getEdges()) {
        Edge* edge = graph_.getEdge(edgePair.first);
        if (edge) {
            edge->setCarCount(0);
        }
    }
}

} // namespace nav
