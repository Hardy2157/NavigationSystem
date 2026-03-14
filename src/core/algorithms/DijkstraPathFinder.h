#ifndef DIJKSTRAPATHFINDER_H
#define DIJKSTRAPATHFINDER_H

#include "core/algorithms/PathFinder.h"
#include <unordered_map>

namespace nav {

// Dijkstra's shortest path algorithm
// Uses edge length as the cost function
class DijkstraPathFinder : public PathFinder {
public:
    DijkstraPathFinder() = default;

    PathResult findPath(const Graph& graph, Node::Id start, Node::Id end) override;

private:
    // Reconstruct the path from start to end using predecessor map
    void reconstructPath(const Graph& graph,
                        Node::Id start,
                        Node::Id end,
                        const std::unordered_map<Node::Id, Edge::Id>& predecessor,
                        PathResult& result) const;
};

} // namespace nav

#endif // DIJKSTRAPATHFINDER_H
