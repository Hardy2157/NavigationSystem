#ifndef EDGEITEM_H
#define EDGEITEM_H

#include <QGraphicsLineItem>
#include "core/graph/Edge.h"

namespace nav {

class EdgeItem : public QGraphicsLineItem {
public:
    explicit EdgeItem(Edge::Id edgeId, const Point2D& from, const Point2D& to,
                      RoadClass roadClass = RoadClass::Local,
                      QGraphicsItem* parent = nullptr);

    Edge::Id getEdgeId() const { return edgeId_; }
    RoadClass getRoadClass() const { return roadClass_; }

    // 根据拥堵状态更新视觉样式
    // status: 0 = 绿色（畅通）, 1 = 黄色（中等）, 2 = 红色（拥堵）
    void updateStyle(int congestionStatus);

    // 根据道路等级恢复默认样式
    void applyBaseStyle();

private:
    Edge::Id edgeId_;
    RoadClass roadClass_;
};

} // namespace nav

#endif // EDGEITEM_H
