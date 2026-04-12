#include "gui/items/ClusterItem.h"
#include <QPainter>
#include <QFont>
#include <cmath>

namespace nav {

ClusterItem::ClusterItem(const Point2D& position, size_t memberCount,
                         QGraphicsItem* parent)
    : QGraphicsEllipseItem(parent)
    , memberCount_(memberCount)
    , visualRadius_(std::max(6.0, std::min(12.0, 4.0 + std::log2(static_cast<double>(memberCount)))))
{
    setPos(position.x, position.y);

    // 关键：忽略视图变换，始终以设备像素坐标绘制
    setFlag(QGraphicsItem::ItemIgnoresTransformations, true);

    // 半透明蓝灰色
    setBrush(QBrush(QColor(100, 130, 180, 160)));
    setPen(QPen(QColor(70, 90, 130), 1.0));

    // Z 值：在边之上，接近节点层
    setZValue(11.0);
}

QRectF ClusterItem::boundingRect() const {
    return QRectF(-HITBOX_RADIUS, -HITBOX_RADIUS,
                  HITBOX_RADIUS * 2.0, HITBOX_RADIUS * 2.0);
}

QPainterPath ClusterItem::shape() const {
    QPainterPath path;
    path.addEllipse(-HITBOX_RADIUS, -HITBOX_RADIUS,
                    HITBOX_RADIUS * 2.0, HITBOX_RADIUS * 2.0);
    return path;
}

void ClusterItem::paint(QPainter* painter,
                        const QStyleOptionGraphicsItem* option,
                        QWidget* widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // 固定视觉大小绘制（不随缩放变化）
    painter->setBrush(brush());
    painter->setPen(pen());
    painter->drawEllipse(QRectF(-visualRadius_, -visualRadius_,
                                 visualRadius_ * 2.0, visualRadius_ * 2.0));

    // 若成员数 > 5 则显示数字标签
    if (memberCount_ > 5) {
        painter->setPen(Qt::white);
        QFont font;
        font.setPixelSize(static_cast<int>(visualRadius_));
        font.setBold(true);
        painter->setFont(font);

        QString text = (memberCount_ > 999)
            ? QString::number(memberCount_ / 1000) + "k"
            : QString::number(memberCount_);
        QRectF textRect(-visualRadius_, -visualRadius_,
                        visualRadius_ * 2.0, visualRadius_ * 2.0);
        painter->drawText(textRect, Qt::AlignCenter, text);
    }
}

} // namespace nav
