#include "gui/items/EdgeItem.h"
#include "gui/items/RoadStyle.h"
#include <QPen>

namespace nav {

EdgeItem::EdgeItem(Edge::Id edgeId, const Point2D& from, const Point2D& to,
                   RoadClass roadClass, QGraphicsItem* parent)
    : QGraphicsLineItem(parent)
    , edgeId_(edgeId)
    , roadClass_(roadClass)
{
    setLine(from.x, from.y, to.x, to.y);

    // 根据道路等级设置默认样式
    applyBaseStyle();
}

void EdgeItem::applyBaseStyle() {
    RoadStyle style = RoadStyle::forRoadClass(roadClass_);
    QPen pen(style.color);
    pen.setWidthF(style.width);
    pen.setCapStyle(Qt::RoundCap);
    setPen(pen);
    setZValue(style.zValue);
}

void EdgeItem::updateStyle(int congestionStatus) {
    if (congestionStatus == 0) {
        applyBaseStyle();
        return;
    }

    QColor color;
    switch (congestionStatus) {
        case 1:  // 黄色 - 中等交通
            color = QColor(255, 193, 7);   // Material 琥珀色 500
            break;
        case 2:  // 红色 - 拥堵
            color = QColor(244, 67, 54);   // Material 红色 500
            break;
        default:
            color = QColor(158, 158, 158); // 灰色
            break;
    }

    double width = RoadStyle::congestionWidth(roadClass_, congestionStatus);
    QPen pen(color);
    pen.setWidthF(width);
    pen.setCapStyle(Qt::RoundCap);
    setPen(pen);
}

} // namespace nav
