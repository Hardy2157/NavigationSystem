#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include "core/graph/Node.h"

namespace nav {

// Axis-Aligned Bounding Box (AABB)
class BoundingBox {
public:
    BoundingBox();
    BoundingBox(double minX, double minY, double maxX, double maxY);

    // Getters
    double getMinX() const { return minX_; }
    double getMinY() const { return minY_; }
    double getMaxX() const { return maxX_; }
    double getMaxY() const { return maxY_; }
    double getWidth() const { return maxX_ - minX_; }
    double getHeight() const { return maxY_ - minY_; }
    Point2D getCenter() const { return Point2D((minX_ + maxX_) / 2.0, (minY_ + maxY_) / 2.0); }

    // Spatial queries
    bool contains(const Point2D& point) const;
    bool intersects(const BoundingBox& other) const;

    // Quadrant subdivision (for QuadTree)
    BoundingBox getNorthWest() const;
    BoundingBox getNorthEast() const;
    BoundingBox getSouthWest() const;
    BoundingBox getSouthEast() const;

private:
    double minX_;
    double minY_;
    double maxX_;
    double maxY_;
};

} // namespace nav

#endif // BOUNDINGBOX_H
