#ifndef PATHFINDER_H
#define PATHFINDER_H

#include "core/graph/Graph.h"
#include <vector>

namespace nav {

// Result of a pathfinding query
struct PathResult {
    std::vector<Node::Id> pathNodes;  // Sequence of nodes from start to end
    std::vector<Edge::Id> pathEdges;  // Sequence of edges used (for highlighting)
    double totalCost;                  // Total cost/distance of the path
    bool found;                        // Whether a path was found

    PathResult()
        : totalCost(0.0)
        , found(false)
    {}
};

// Abstract interface for pathfinding algorithms
class PathFinder {
public:
    virtual ~PathFinder() = default;

    // Find a path from start to end node
    virtual PathResult findPath(const Graph& graph, Node::Id start, Node::Id end) = 0;
};

} // namespace nav

#endif // PATHFINDER_H
