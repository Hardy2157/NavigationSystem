#ifndef PATHFINDER_H
#define PATHFINDER_H

#include "core/graph/Graph.h"
#include <vector>

namespace nav {

// 路径查找查询的结果
struct PathResult {
    std::vector<Node::Id> pathNodes;  // 从起点到终点的节点序列
    std::vector<Edge::Id> pathEdges;  // 使用的边序列（用于高亮显示）
    double totalCost;                  // 路径的总开销/距离
    bool found;                        // 是否找到路径

    PathResult()
        : totalCost(0.0)
        , found(false)
    {}
};

// 路径查找算法的抽象接口
class PathFinder {
public:
    virtual ~PathFinder() = default;

    // 查找从起点到终点的路径
    virtual PathResult findPath(const Graph& graph, Node::Id start, Node::Id end) = 0;
};

} // namespace nav

#endif // PATHFINDER_H
