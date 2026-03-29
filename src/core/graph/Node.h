#ifndef NODE_H
#define NODE_H

#include <cstdint>
#include <cmath>

namespace nav {

// 二维点结构
struct Point2D {
    double x;
    double y;

    Point2D() : x(0.0), y(0.0) {}
    Point2D(double x_, double y_) : x(x_), y(y_) {}

    // 计算到另一点的欧几里得距离
    double distanceTo(const Point2D& other) const {
        double dx = x - other.x;
        double dy = y - other.y;
        return std::sqrt(dx * dx + dy * dy);
    }

    // 相等运算符
    bool operator==(const Point2D& other) const {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Point2D& other) const {
        return !(*this == other);
    }
};

// 节点类，表示导航图中的位置
class Node {
public:
    using Id = uint32_t;
    static constexpr Id INVALID_ID = UINT32_MAX;

    Node();
    Node(Id id, const Point2D& position);

    // 获取器
    Id getId() const { return id_; }
    const Point2D& getPosition() const { return position_; }

    // 设置器
    void setPosition(const Point2D& position) { position_ = position; }

private:
    Id id_;
    Point2D position_;
};

} // namespace nav

#endif // NODE_H
