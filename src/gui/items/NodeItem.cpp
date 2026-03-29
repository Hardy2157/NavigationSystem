#include "gui/items/NodeItem.h"
#include <QBrush>
#include <QPen>
#include <QPainter>

namespace nav {

NodeItem::NodeItem(Node::Id nodeId, const Point2D& position, QGraphicsItem* parent)
    : QGraphicsEllipseItem(parent)
    , nodeId_(nodeId)
{
    // 设置位置以节点坐标为中心
    setPos(position.x, position.y);

    // 样式：深灰色填充，无边框（默认外观）
    setBrush(QBrush(QColor(60, 60, 60)));
    setPen(Qt::NoPen);

    // 关键：设置高 Z 值，使节点始终位于边之上
    // 这确保节点上的点击不会被底层边窃取
    setZValue(10.0);

    // 启用鼠标事件
    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemIsSelectable, false);
}

QRectF NodeItem::boundingRect() const {
    // 关键：返回包含整个可点击区域的矩形
    // Qt 的 BSP 树使用 boundingRect 进行事件过滤
    // 如果 boundingRect 很小，即使在 shape() 内，外部的点击也会被忽略
    return QRectF(-HITBOX_RADIUS, -HITBOX_RADIUS,
                  HITBOX_RADIUS * 2.0, HITBOX_RADIUS * 2.0);
}

QPainterPath NodeItem::shape() const {
    // 返回用于可点击碰撞框的大椭圆（20px 直径）
    QPainterPath path;
    path.addEllipse(-HITBOX_RADIUS, -HITBOX_RADIUS,
                    HITBOX_RADIUS * 2.0, HITBOX_RADIUS * 2.0);
    return path;
}

void NodeItem::setHighlighted(bool highlighted) {
    if (m_isHighlighted != highlighted) {
        m_isHighlighted = highlighted;
        update();  // 触发重绘
    }
}

void NodeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // 高亮时使用较大半径，正常时使用较小半径
    double radius = m_isHighlighted ? HIGHLIGHT_RADIUS : NORMAL_RADIUS;

    painter->setBrush(brush());
    painter->setPen(pen());
    painter->drawEllipse(QRectF(-radius, -radius, radius * 2.0, radius * 2.0));
}

} // namespace nav
