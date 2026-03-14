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
    // Check if point is within bounds
    if (!bounds_.contains(position)) {
        return false;
    }

    // If we have capacity and haven't subdivided, add here
    if (entries_.size() < MAX_CAPACITY && !divided_) {
        entries_.emplace_back(nodeId, position);
        return true;
    }

    // Otherwise, subdivide if needed
    if (!divided_) {
        subdivide();
    }

    // Insert into appropriate child
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

    // Move existing entries to children
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
    std::vector<DistanceEntry> candidates;
    findKNearestHelper(query, k, candidates);

    // Sort by distance (ascending)
    std::sort(candidates.begin(), candidates.end(),
              [](const DistanceEntry& a, const DistanceEntry& b) {
                  return a.distance < b.distance;
              });

    // Extract the k nearest IDs
    std::vector<Node::Id> result;
    size_t count = std::min(k, candidates.size());
    result.reserve(count);
    for (size_t i = 0; i < count; ++i) {
        result.push_back(candidates[i].id);
    }

    return result;
}

void QuadTree::findKNearestHelper(const Point2D& query, size_t k,
                                  std::vector<DistanceEntry>& candidates) const {
    // Add all entries in this node
    for (const auto& entry : entries_) {
        double dist = query.distanceTo(entry.position);
        candidates.emplace_back(entry.id, dist);
    }

    // If not divided, we're done
    if (!divided_) {
        return;
    }

    // Calculate distances to each child's bounding box
    struct ChildDistance {
        QuadTree* child;
        double distance;

        bool operator<(const ChildDistance& other) const {
            return distance > other.distance; // Min heap
        }
    };

    std::priority_queue<ChildDistance> childQueue;

    auto addChild = [&](QuadTree* child) {
        if (!child) return;

        // Calculate minimum distance from query point to child's bounding box
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

    // Visit children in order of distance to their bounding boxes
    while (!childQueue.empty()) {
        auto childDist = childQueue.top();
        childQueue.pop();

        // Prune if we have k candidates and this child is farther than the k-th candidate
        if (candidates.size() >= k) {
            // Find the k-th smallest distance
            std::vector<double> distances;
            distances.reserve(candidates.size());
            for (const auto& c : candidates) {
                distances.push_back(c.distance);
            }
            std::nth_element(distances.begin(), distances.begin() + k - 1, distances.end());
            double kthDistance = distances[k - 1];

            if (childDist.distance > kthDistance) {
                continue; // Prune this subtree
            }
        }

        childDist.child->findKNearestHelper(query, k, candidates);
    }
}

std::vector<Node::Id> QuadTree::queryRange(const BoundingBox& range) const {
    std::vector<Node::Id> result;
    queryRangeHelper(range, result);
    return result;
}

void QuadTree::queryRangeHelper(const BoundingBox& range, std::vector<Node::Id>& result) const {
    // Check if this node's bounds intersect with the query range
    if (!bounds_.intersects(range)) {
        return;
    }

    // Add all entries that are within the range
    for (const auto& entry : entries_) {
        if (range.contains(entry.position)) {
            result.push_back(entry.id);
        }
    }

    // Recursively search children
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
