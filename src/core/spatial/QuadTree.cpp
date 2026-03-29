#include "core/spatial/QuadTree.h"
#include <queue>
#include <algorithm>
#include <cmath>

namespace nav {

QuadTree::QuadTree(const BoundingBox& bounds)
    : bounds_(bounds)
    , divided_(false)
{}

bool QuadTree::insert(Node::Id nodeId, const Point2D& position) {
    // 检查点是否在边界内
    if (!bounds_.contains(position)) {
        return false;
    }

    // 如果有容量且未细分，则在此处添加
    if (entries_.size() < MAX_CAPACITY && !divided_) {
        entries_.emplace_back(nodeId, position);
        return true;
    }

    // 否则，如果需要则细分
    if (!divided_) {
        subdivide();
    }

    // 插入到适当的子节点
    if (northWest_->insert(nodeId, position)) return true;
    if (northEast_->insert(nodeId, position)) return true;
    if (southWest_->insert(nodeId, position)) return true;
    if (southEast_->insert(nodeId, position)) return true;

    return false;
}

void QuadTree::subdivide() {
    northWest_ = std::make_unique<QuadTree>(bounds_.getNorthWest());
    northEast_ = std::make_unique<QuadTree>(bounds_.getNorthEast());
    southWest_ = std::make_unique<QuadTree>(bounds_.getSouthWest());
    southEast_ = std::make_unique<QuadTree>(bounds_.getSouthEast());

    // 将现有条目移动到子节点
    for (const auto& entry : entries_) {
        northWest_->insert(entry.id, entry.position);
        northEast_->insert(entry.id, entry.position);
        southWest_->insert(entry.id, entry.position);
        southEast_->insert(entry.id, entry.position);
    }

    entries_.clear();
    divided_ = true;
}

std::vector<Node::Id> QuadTree::findKNearest(const Point2D& query, size_t k) const {
    // 大小为 k 的最大堆：顶部元素是 k 个最近元素中最远的
    std::priority_queue<DistanceEntry> maxHeap;
    findKNearestHelper(query, k, maxHeap);

    // 从堆中提取结果（最远的优先，然后反转）
    std::vector<Node::Id> result;
    result.reserve(maxHeap.size());
    while (!maxHeap.empty()) {
        result.push_back(maxHeap.top().id);
        maxHeap.pop();
    }
    std::reverse(result.begin(), result.end());  // 最近的优先
    return result;
}

void QuadTree::findKNearestHelper(const Point2D& query, size_t k,
                                  std::priority_queue<DistanceEntry>& maxHeap) const {
    // 处理此节点中的条目
    for (const auto& entry : entries_) {
        double dist = query.distanceTo(entry.position);
        if (maxHeap.size() < k) {
            maxHeap.push(DistanceEntry(entry.id, dist));
        } else if (dist < maxHeap.top().distance) {
            maxHeap.pop();
            maxHeap.push(DistanceEntry(entry.id, dist));
        }
    }

    // 如果未细分，则完成
    if (!divided_) {
        return;
    }

    // 计算到每个子节点边界框的距离
    struct ChildDistance {
        QuadTree* child;
        double distance;

        bool operator<(const ChildDistance& other) const {
            return distance > other.distance; // 最小堆
        }
    };

    std::priority_queue<ChildDistance> childQueue;

    auto addChild = [&](QuadTree* child) {
        if (!child) return;

        // 计算从查询点到子节点边界框的最小距离
        double dx = 0.0;
        double dy = 0.0;

        if (query.x < child->bounds_.getMinX()) {
            dx = child->bounds_.getMinX() - query.x;
        } else if (query.x > child->bounds_.getMaxX()) {
            dx = query.x - child->bounds_.getMaxX();
        }

        if (query.y < child->bounds_.getMinY()) {
            dy = child->bounds_.getMinY() - query.y;
        } else if (query.y > child->bounds_.getMaxY()) {
            dy = query.y - child->bounds_.getMaxY();
        }

        double dist = std::sqrt(dx * dx + dy * dy);
        childQueue.push({child, dist});
    };

    addChild(northWest_.get());
    addChild(northEast_.get());
    addChild(southWest_.get());
    addChild(southEast_.get());

    // 按距离顺序访问子节点，使用最大堆进行剪枝
    while (!childQueue.empty()) {
        auto childDist = childQueue.top();
        childQueue.pop();

        // 剪枝：如果堆已满且子节点比最差候选者更远
        if (maxHeap.size() >= k && childDist.distance > maxHeap.top().distance) {
            continue;
        }

        childDist.child->findKNearestHelper(query, k, maxHeap);
    }
}

std::vector<Node::Id> QuadTree::queryRange(const BoundingBox& range) const {
    std::vector<Node::Id> result;
    queryRangeHelper(range, result);
    return result;
}

void QuadTree::queryRangeHelper(const BoundingBox& range, std::vector<Node::Id>& result) const {
    // 检查此节点的边界是否与查询范围相交
    if (!bounds_.intersects(range)) {
        return;
    }

    // 添加范围内的所有条目
    for (const auto& entry : entries_) {
        if (range.contains(entry.position)) {
            result.push_back(entry.id);
        }
    }

    // 递归搜索子节点
    if (divided_) {
        northWest_->queryRangeHelper(range, result);
        northEast_->queryRangeHelper(range, result);
        southWest_->queryRangeHelper(range, result);
        southEast_->queryRangeHelper(range, result);
    }
}

void QuadTree::clear() {
    entries_.clear();
    divided_ = false;
    northWest_.reset();
    northEast_.reset();
    southWest_.reset();
    southEast_.reset();
}

} // namespace nav
