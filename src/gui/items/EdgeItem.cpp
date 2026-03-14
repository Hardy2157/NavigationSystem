#include "gui/items/EdgeItem.h"
#include <QPen>

namespace nav {

EdgeItem::EdgeItem(Edge::Id edgeId, const Point2D& from, const Point2D& to, QGraphicsItem* parent)
    : QGraphicsLineItem(parent)
    , edgeId_(edgeId)
{
    setLine(from.x, from.y, to.x, to.y);

    // Default style: green (free flow)
    updateStyle(0);

    // Set Z-value so edges appear below nodes
    setZValue(0.0);
}

void EdgeItem::updateStyle(int congestionStatus) {
    QColor color;
    double width;

    switch (congestionStatus) {
        case 0:  // Green - free flow
            color = QColor(76, 175, 80);   // Material Green 500
            width = 1.5;
            break;
        case 1:  // Yellow - moderate traffic
            color = QColor(255, 193, 7);   // Material Amber 500
            width = 2.0;
            break;
        case 2:  // Red - congested
            color = QColor(244, 67, 54);   // Material Red 500
            width = 2.5;
            break;
        default:
            color = QColor(158, 158, 158); // Gray
            width = 1.0;
            break;
    }

    QPen pen(color);
    pen.setWidthF(width);
    pen.setCapStyle(Qt::RoundCap);
    setPen(pen);
}

} // namespace nav
