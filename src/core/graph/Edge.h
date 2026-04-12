#ifndef EDGE_H
#define EDGE_H

#include "core/graph/Node.h"
#include <cstdint>

namespace nav {

// 道路等级枚举
enum class RoadClass : uint8_t {
    Arterial  = 0,  // 主干道
    Secondary = 1,  // 次干道
    Local     = 2   // 支路
};

// 道路等级速度因子（主干道行驶更快）
inline double roadClassSpeedFactor(RoadClass rc) {
    switch (rc) {
        case RoadClass::Arterial:  return 2.0;
        case RoadClass::Secondary: return 1.5;
        case RoadClass::Local:
        default:                   return 1.0;
    }
}

// 边类，表示两个节点之间的连接
class Edge {
public:
    using Id = uint32_t;
    static constexpr Id INVALID_ID = UINT32_MAX;

    Edge();
    Edge(Id id, Node::Id source, Node::Id target, double length);

    // 获取器
    Id getId() const { return id_; }
    Node::Id getSource() const { return source_; }
    Node::Id getTarget() const { return target_; }
    double getLength() const { return length_; }
    double getCapacity() const { return capacity_; }
    uint32_t getCarCount() const { return carCount_; }
    RoadClass getRoadClass() const { return roadClass_; }

    // 设置器
    void setCapacity(double capacity) { capacity_ = capacity; }
    void setCarCount(uint32_t count) { carCount_ = count; }
    void setRoadClass(RoadClass rc) { roadClass_ = rc; }

    // 交通管理
    void incrementCarCount() { ++carCount_; }
    void decrementCarCount() { if (carCount_ > 0) --carCount_; }

private:
    Id id_;
    Node::Id source_;
    Node::Id target_;
    double length_;        // 边的物理长度
    double capacity_;      // 最大车辆容量 (v)
    uint32_t carCount_;    // 当前边上的车辆数 (n)
    RoadClass roadClass_ = RoadClass::Local;  // 道路等级
};

} // namespace nav

#endif // EDGE_H
