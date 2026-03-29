#ifndef DIJKSTRAPATHFINDER_H
#define DIJKSTRAPATHFINDER_H

#include "core/algorithms/PathFinder.h"
#include <unordered_map>

namespace nav {

// Dijkstra 最短路径算法
// 使用边长度作为开销函数
class DijkstraPathFinder : public PathFinder {
public:
    DijkstraPathFinder() = default;

    PathResult findPath(const Graph& graph, Node::Id start, Node::Id end) override;

private:
    // 使用前驱映射重建从起点到终点的路径
    void reconstructPath(const Graph& graph,
                        Node::Id start,
                        Node::Id end,
                        const std::unordered_map<Node::Id, Edge::Id>& predecessor,
                        PathResult& result) const;
};

} // namespace nav

#endif // DIJKSTRAPATHFINDER_H
