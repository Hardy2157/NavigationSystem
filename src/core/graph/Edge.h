#ifndef EDGE_H
#define EDGE_H

#include "core/graph/Node.h"
#include <cstdint>

namespace nav {

// Edge class representing a connection between two nodes
class Edge {
public:
    using Id = uint32_t;
    static constexpr Id INVALID_ID = UINT32_MAX;

    Edge();
    Edge(Id id, Node::Id source, Node::Id target, double length);

    // Getters
    Id getId() const { return id_; }
    Node::Id getSource() const { return source_; }
    Node::Id getTarget() const { return target_; }
    double getLength() const { return length_; }
    double getCapacity() const { return capacity_; }
    uint32_t getCarCount() const { return carCount_; }

    // Setters
    void setCapacity(double capacity) { capacity_ = capacity; }
    void setCarCount(uint32_t count) { carCount_ = count; }

    // Traffic management
    void incrementCarCount() { ++carCount_; }
    void decrementCarCount() { if (carCount_ > 0) --carCount_; }

private:
    Id id_;
    Node::Id source_;
    Node::Id target_;
    double length_;        // Physical length of the edge
    double capacity_;      // Maximum vehicle capacity (v)
    uint32_t carCount_;    // Current number of cars on this edge (n)
};

} // namespace nav

#endif // EDGE_H
