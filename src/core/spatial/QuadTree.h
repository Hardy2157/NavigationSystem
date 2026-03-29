#ifndef QUADTREE_H
#define QUADTREE_H

#include "core/spatial/BoundingBox.h"
#include "core/graph/Node.h"
#include <vector>
#include <memory>
#include <queue>

namespace nav {

// 用于节点高效空间索引的四叉树
class QuadTree {
public:
    static constexpr size_t MAX_CAPACITY = 4;

    // 构造函数
    explicit QuadTree(const BoundingBox& bounds);

    // 将节点插入树中
    bool insert(Node::Id nodeId, const Point2D& position);

    // 查找查询点的 k 个最近邻
    std::vector<Node::Id> findKNearest(const Point2D& query, size_t k) const;

    // 查询包围盒内的所有节点
    std::vector<Node::Id> queryRange(const BoundingBox& range) const;

    // 清空树
    void clear();

private:
    struct NodeEntry {
        Node::Id id;
        Point2D position;

        NodeEntry(Node::Id nodeId, const Point2D& pos)
            : id(nodeId), position(pos) {}
    };

    // k-最近邻搜索的辅助结构（最大堆）
    struct DistanceEntry {
        Node::Id id;
        double distance;

        DistanceEntry(Node::Id nodeId, double dist)
            : id(nodeId), distance(dist) {}

        // 用于最大堆（priority_queue 默认使用 less，所以我们反转）
        bool operator<(const DistanceEntry& other) const {
            return distance < other.distance;
        }
    };

    // 将此节点细分为 4 个子节点
    void subdivide();

    // 使用最大堆的 k-最近邻搜索辅助函数
    void findKNearestHelper(const Point2D& query, size_t k,
                           std::priority_queue<DistanceEntry>& maxHeap) const;

    // 范围查询辅助函数
    void queryRangeHelper(const BoundingBox& range, std::vector<Node::Id>& result) const;

    BoundingBox bounds_;
    std::vector<NodeEntry> entries_;
    bool divided_;

    // 子节点（西北、东北、西南、东南）
    std::unique_ptr<QuadTree> northWest_;
    std::unique_ptr<QuadTree> northEast_;
    std::unique_ptr<QuadTree> southWest_;
    std::unique_ptr<QuadTree> southEast_;
};

} // namespace nav

#endif // QUADTREE_H
