#ifndef NODE_H
#define NODE_H

#include <cstdint>
#include <cmath>

namespace nav {

// 2D Point structure
struct Point2D {
    double x;
    double y;

    Point2D() : x(0.0), y(0.0) {}
    Point2D(double x_, double y_) : x(x_), y(y_) {}

    // Calculate Euclidean distance to another point
    double distanceTo(const Point2D& other) const {
        double dx = x - other.x;
        double dy = y - other.y;
        return std::sqrt(dx * dx + dy * dy);
    }

    // Equality operators
    bool operator==(const Point2D& other) const {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Point2D& other) const {
        return !(*this == other);
    }
};

// Node class representing a location in the navigation graph
class Node {
public:
    using Id = uint32_t;
    static constexpr Id INVALID_ID = UINT32_MAX;

    Node();
    Node(Id id, const Point2D& position);

    // Getters
    Id getId() const { return id_; }
    const Point2D& getPosition() const { return position_; }

    // Setters
    void setPosition(const Point2D& position) { position_ = position; }

private:
    Id id_;
    Point2D position_;
};

} // namespace nav

#endif // NODE_H
