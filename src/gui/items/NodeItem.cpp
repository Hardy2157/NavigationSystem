#include "gui/items/NodeItem.h"
#include <QBrush>
#include <QPen>
#include <QPainter>

namespace nav {

NodeItem::NodeItem(Node::Id nodeId, const Point2D& position, QGraphicsItem* parent)
    : QGraphicsEllipseItem(parent)
    , nodeId_(nodeId)
{
    // Set position centered on the node coordinates
    setPos(position.x, position.y);

    // Style: dark gray fill, no border (default appearance)
    setBrush(QBrush(QColor(60, 60, 60)));
    setPen(Qt::NoPen);

    // CRITICAL: Set high Z-value so nodes are always above edges
    // This ensures clicks on nodes are not stolen by underlying edges
    setZValue(10.0);

    // Enable mouse events
    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemIsSelectable, false);
}

QRectF NodeItem::boundingRect() const {
    // CRITICAL: Return a rectangle that encompasses the entire clickable area
    // Qt's BSP tree uses boundingRect for event filtering
    // If boundingRect is tiny, clicks outside it are ignored even if inside shape()
    return QRectF(-HITBOX_RADIUS, -HITBOX_RADIUS,
                  HITBOX_RADIUS * 2.0, HITBOX_RADIUS * 2.0);
}

QPainterPath NodeItem::shape() const {
    // Return a large ellipse for the clickable hitbox (20px diameter)
    QPainterPath path;
    path.addEllipse(-HITBOX_RADIUS, -HITBOX_RADIUS,
                    HITBOX_RADIUS * 2.0, HITBOX_RADIUS * 2.0);
    return path;
}

void NodeItem::setHighlighted(bool highlighted) {
    if (m_isHighlighted != highlighted) {
        m_isHighlighted = highlighted;
        update();  // Trigger repaint
    }
}

void NodeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // Use larger radius when highlighted, smaller when normal
    double radius = m_isHighlighted ? HIGHLIGHT_RADIUS : NORMAL_RADIUS;

    painter->setBrush(brush());
    painter->setPen(pen());
    painter->drawEllipse(QRectF(-radius, -radius, radius * 2.0, radius * 2.0));
}

} // namespace nav
