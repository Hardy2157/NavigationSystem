#ifndef NODEITEM_H
#define NODEITEM_H

#include <QGraphicsEllipseItem>
#include <QPainterPath>
#include <QRectF>
#include "core/graph/Node.h"

namespace nav {

class NodeItem : public QGraphicsEllipseItem {
public:
    static constexpr double NORMAL_RADIUS = 2.0;      // Small visual dot (normal state)
    static constexpr double HIGHLIGHT_RADIUS = 8.0;   // Large visual circle (highlighted state)
    static constexpr double HITBOX_RADIUS = 10.0;     // Large clickable area

    explicit NodeItem(Node::Id nodeId, const Point2D& position, QGraphicsItem* parent = nullptr);

    Node::Id getNodeId() const { return nodeId_; }

    // Set highlight state (makes node visually larger)
    void setHighlighted(bool highlighted);
    bool isHighlighted() const { return m_isHighlighted; }

    // Override boundingRect to encompass the entire clickable area (CRITICAL for Qt's BSP tree)
    QRectF boundingRect() const override;

    // Override shape to provide larger click hitbox
    QPainterPath shape() const override;

    // Override paint to draw only the small visual dot
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    Node::Id nodeId_;
    bool m_isHighlighted = false;
};

} // namespace nav

#endif // NODEITEM_H
