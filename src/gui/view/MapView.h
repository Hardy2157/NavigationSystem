#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <QGraphicsView>
#include <QRectF>

namespace nav {

class MapView : public QGraphicsView {
    Q_OBJECT

public:
    explicit MapView(QWidget* parent = nullptr);

    // 缩放以适应所有内容
    void zoomToFit();

    // 根据当前缩放更新 LOD 可见性
    void updateLOD();

    // 获取当前缩放级别
    double getCurrentZoom() const { return currentZoom_; }

    // 聚焦到特定点并使用合理的缩放级别
    void focusOnPoint(double x, double y, double zoomLevel = 2.0);

    // 聚焦到带填充的边界矩形
    void focusOnBounds(const QRectF& bounds, double padding = 50.0);

protected:
    // 重写滚轮事件以在光标处居中缩放
    void wheelEvent(QWheelEvent* event) override;

private:
    static constexpr double ZOOM_FACTOR = 1.15;
    static constexpr double MIN_ZOOM = 0.01;
    static constexpr double MAX_ZOOM = 100.0;
    static constexpr double LOD_THRESHOLD = 0.5;  // 在此缩放级别以下隐藏节点

    double currentZoom_ = 1.0;
};

} // namespace nav

#endif // MAPVIEW_H
