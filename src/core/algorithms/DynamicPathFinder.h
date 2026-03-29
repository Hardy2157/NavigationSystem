#ifndef DYNAMICPATHFINDER_H
#define DYNAMICPATHFINDER_H

#include "core/algorithms/PathFinder.h"
#include <unordered_map>

namespace nav {

// 动态路径查找器，使用交通感知的行驶时间作为开销
// 使用 TrafficModel::calculateTravelTime() 而不是原始边长度
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
