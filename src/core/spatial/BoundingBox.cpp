#include "core/spatial/BoundingBox.h"

namespace nav {

BoundingBox::BoundingBox()
    : minX_(0.0), minY_(0.0), maxX_(0.0), maxY_(0.0)
{}

BoundingBox::BoundingBox(double minX, double minY, double maxX, double maxY)
    : minX_(minX), minY_(minY), maxX_(maxX), maxY_(maxY)
{}

bool BoundingBox::contains(const Point2D& point) const {
    return point.x >= minX_ && point.x <= maxX_ &&
           point.y >= minY_ && point.y <= maxY_;
}

bool BoundingBox::intersects(const BoundingBox& other) const {
    // Two AABBs intersect if they overlap on both axes
    return !(other.minX_ > maxX_ || other.maxX_ < minX_ ||
             other.minY_ > maxY_ || other.maxY_ < minY_);
}

BoundingBox BoundingBox::getNorthWest() const {
    double centerX = (minX_ + maxX_) / 2.0;
    double centerY = (minY_ + maxY_) / 2.0;
    return BoundingBox(minX_, centerY, centerX, maxY_);
}

BoundingBox BoundingBox::getNorthEast() const {
    double centerX = (minX_ + maxX_) / 2.0;
    double centerY = (minY_ + maxY_) / 2.0;
    return BoundingBox(centerX, centerY, maxX_, maxY_);
}

BoundingBox BoundingBox::getSouthWest() const {
    double centerX = (minX_ + maxX_) / 2.0;
    double centerY = (minY_ + maxY_) / 2.0;
    return BoundingBox(minX_, minY_, centerX, centerY);
}

BoundingBox BoundingBox::getSouthEast() const {
    double centerX = (minX_ + maxX_) / 2.0;
    double centerY = (minY_ + maxY_) / 2.0;
    return BoundingBox(centerX, minY_, maxX_, centerY);
}

} // namespace nav
