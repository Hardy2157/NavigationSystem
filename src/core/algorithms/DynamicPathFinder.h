#ifndef DYNAMICPATHFINDER_H
#define DYNAMICPATHFINDER_H

#include "core/algorithms/PathFinder.h"
#include <unordered_map>

namespace nav {

// Dynamic pathfinder that uses traffic-aware travel time as cost
// Uses TrafficModel::calculateTravelTime() instead of raw edge length
class DynamicPathFinder : public PathFinder {
public:
    DynamicPathFinder() = default;

    PathResult findPath(const Graph& graph, Node::Id start, Node::Id end) override;

private:
    void reconstructPath(const Graph& graph,
                        Node::Id start,
                        Node::Id end,
                        const std::unordered_map<Node::Id, Edge::Id>& predecessor,
                        PathResult& result) const;
};

} // namespace nav

#endif // DYNAMICPATHFINDER_H
