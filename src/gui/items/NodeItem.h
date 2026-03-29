#ifndef NODEITEM_H
#define NODEITEM_H

#include <QGraphicsEllipseItem>
#include <QPainterPath>
#include <QRectF>
#include "core/graph/Node.h"

namespace nav {

class NodeItem : public QGraphicsEllipseItem {
public:
    static constexpr double NORMAL_RADIUS = 2.0;      // 小视觉点（正常状态）
    static constexpr double HIGHLIGHT_RADIUS = 8.0;   // 大视觉圆（高亮状态）
    static constexpr double HITBOX_RADIUS = 10.0;     // 大可点击区域

    explicit NodeItem(Node::Id nodeId, const Point2D& position, QGraphicsItem* parent = nullptr);

    Node::Id getNodeId() const { return nodeId_; }

    // 设置高亮状态（使节点在视觉上更大）
    void setHighlighted(bool highlighted);
    bool isHighlighted() const { return m_isHighlighted; }

    // 重写 boundingRect 以包含整个可点击区域（对 Qt 的 BSP 树至关重要）
    QRectF boundingRect() const override;

    // 重写 shape 以提供更大的点击碰撞框
    QPainterPath shape() const override;

    // 重写 paint 以仅绘制小视觉点
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    Node::Id nodeId_;
    bool m_isHighlighted = false;
};

} // namespace nav

#endif // NODEITEM_H
