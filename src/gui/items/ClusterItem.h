#ifndef CLUSTERITEM_H
#define CLUSTERITEM_H

#include <QGraphicsEllipseItem>
#include <QPainterPath>
#include <QRectF>
#include "core/graph/Node.h"

namespace nav {

// 聚类代表点的可视化项（固定屏幕大小，类似地图标记）
class ClusterItem : public QGraphicsEllipseItem {
public:
    static constexpr double VISUAL_RADIUS = 8.0;
    static constexpr double HITBOX_RADIUS = 15.0;

    ClusterItem(const Point2D& position, size_t memberCount,
                QGraphicsItem* parent = nullptr);

    size_t getMemberCount() const { return memberCount_; }

    // 重写以提供固定大小的碰撞区域（对 Qt BSP 树至关重要）
    QRectF boundingRect() const override;
    QPainterPath shape() const override;

    // 重写以固定视觉大小绘制（不随缩放变化）
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

private:
    size_t memberCount_;
    double visualRadius_;  // 根据 memberCount 计算的视觉半径
};

} // namespace nav

#endif // CLUSTERITEM_H
