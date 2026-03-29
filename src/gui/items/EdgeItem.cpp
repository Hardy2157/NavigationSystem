#include "gui/items/EdgeItem.h"
#include <QPen>

namespace nav {

EdgeItem::EdgeItem(Edge::Id edgeId, const Point2D& from, const Point2D& to, QGraphicsItem* parent)
    : QGraphicsLineItem(parent)
    , edgeId_(edgeId)
{
    setLine(from.x, from.y, to.x, to.y);

    // 默认样式：绿色（畅通）
    updateStyle(0);

    // 设置 Z 值使边显示在节点下方
    setZValue(0.0);
}

void EdgeItem::updateStyle(int congestionStatus) {
    QColor color;
    double width;

    switch (congestionStatus) {
        case 0:  // 绿色 - 畅通
            color = QColor(76, 175, 80);   // Material 绿色 500
            width = 1.5;
            break;
        case 1:  // 黄色 - 中等交通
            color = QColor(255, 193, 7);   // Material 琥珀色 500
            width = 2.0;
            break;
        case 2:  // 红色 - 拥堵
            color = QColor(244, 67, 54);   // Material 红色 500
            width = 2.5;
            break;
        default:
            color = QColor(158, 158, 158); // 灰色
            width = 1.0;
            break;
    }

    QPen pen(color);
    pen.setWidthF(width);
    pen.setCapStyle(Qt::RoundCap);
    setPen(pen);
}

} // namespace nav
