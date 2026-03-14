#ifndef EDGEITEM_H
#define EDGEITEM_H

#include <QGraphicsLineItem>
#include "core/graph/Edge.h"

namespace nav {

class EdgeItem : public QGraphicsLineItem {
public:
    explicit EdgeItem(Edge::Id edgeId, const Point2D& from, const Point2D& to, QGraphicsItem* parent = nullptr);

    Edge::Id getEdgeId() const { return edgeId_; }

    // Update visual style based on congestion status
    // status: 0 = Green (free), 1 = Yellow (moderate), 2 = Red (congested)
    void updateStyle(int congestionStatus);

private:
    Edge::Id edgeId_;
};

} // namespace nav

#endif // EDGEITEM_H
