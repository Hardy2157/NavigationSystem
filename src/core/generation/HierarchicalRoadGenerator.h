#ifndef HIERARCHICALROADGENERATOR_H
#define HIERARCHICALROADGENERATOR_H

#include "core/generation/MapGenerator.h"
#include <random>
#include <vector>

namespace nav {

// 分层道路网络生成器
// 自顶向下生成：主干道 -> 次干道 -> 支路
class HierarchicalRoadGenerator : public MapGenerator {
public:
    HierarchicalRoadGenerator();
    explicit HierarchicalRoadGenerator(unsigned int seed);

    void generate(Graph& graph, int numNodes, double width, double height) override;

    // 配置参数
    void setArterialSpacing(double spacing) { arterialSpacing_ = spacing; }
    void setSecondarySpacing(double spacing) { secondarySpacing_ = spacing; }

private:
    std::mt19937 rng_;
    double arterialSpacing_ = 0.0;   // 0 = 自动计算
    double secondarySpacing_ = 0.0;  // 0 = 自动计算

    // 内部生成阶段
    void generateArterials(Graph& graph,
        double width, double height, int targetNodes,
        std::vector<Node::Id>& arterialNodes);

    void generateSecondaries(Graph& graph,
        double width, double height, int targetNodes,
        const std::vector<Node::Id>& arterialNodes,
        std::vector<Node::Id>& secondaryNodes);

    void generateLocals(Graph& graph,
        int remainingNodes, double width, double height,
        const std::vector<Node::Id>& arterialNodes,
        const std::vector<Node::Id>& secondaryNodes);

    // 确保全图连通
    void ensureConnectivity(Graph& graph);

    // 在候选节点中找到距 pos 最近的节点
    Node::Id findNearestNode(const Graph& graph,
        const Point2D& pos,
        const std::vector<Node::Id>& candidates) const;
};

} // namespace nav

#endif // HIERARCHICALROADGENERATOR_H
