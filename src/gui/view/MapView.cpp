#include "gui/view/MapView.h"
#include "gui/view/MapScene.h"
#include "gui/items/NodeItem.h"
#include <QWheelEvent>
#include <QScrollBar>

namespace nav {

MapView::MapView(QWidget* parent)
    : QGraphicsView(parent)
{
    // 启用鼠标拖动平移
    setDragMode(ScrollHandDrag);

    // 启用抗锯齿以实现更平滑的渲染
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);

    // 优化渲染
    setViewportUpdateMode(SmartViewportUpdate);
    setOptimizationFlag(DontSavePainterState);
    setOptimizationFlag(DontAdjustForAntialiasing);

    // 设置变换锚点
    setTransformationAnchor(AnchorUnderMouse);
    setResizeAnchor(AnchorUnderMouse);

    // 允许稍微滚动超出场景边界
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void MapView::zoomToFit() {
    if (scene()) {
        fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
        currentZoom_ = transform().m11();  // 获取当前缩放因子
        updateLOD();
    }
}

void MapView::updateLOD() {
    if (!scene()) return;

    MapScene* mapScene = qobject_cast<MapScene*>(scene());
    if (!mapScene) return;

    // 从变换矩阵获取当前缩放比例
    double scale = transform().m11();

    // 根据缩放级别显示/隐藏节点
    bool showNodes = (scale >= LOD_THRESHOLD);

    // 使用预构建的节点映射而不是迭代所有场景项
    for (const auto& pair : mapScene->getNodeItems()) {
        if (pair.second) {
            pair.second->setVisible(showNodes);
        }
    }
}

void MapView::wheelEvent(QWheelEvent* event) {
    // Calculate zoom factor based on wheel delta
    double factor = (event->angleDelta().y() > 0) ? ZOOM_FACTOR : (1.0 / ZOOM_FACTOR);

    // Calculate new zoom level
    double newZoom = currentZoom_ * factor;

    // Clamp zoom level
    if (newZoom < MIN_ZOOM || newZoom > MAX_ZOOM) {
        return;
    }

    // 应用以光标位置为中心的缩放
    scale(factor, factor);
    currentZoom_ = newZoom;

    // 缩放后更新 LOD
    updateLOD();
}

void MapView::focusOnPoint(double x, double y, double zoomLevel) {
    // 重置变换并设置所需的缩放级别
    resetTransform();
    scale(zoomLevel, zoomLevel);
    currentZoom_ = zoomLevel;

    // 居中到该点
    centerOn(x, y);

    // 更新 LOD
    updateLOD();
}

void MapView::focusOnBounds(const QRectF& bounds, double padding) {
    if (bounds.isEmpty()) return;

    // 为边界添加填充
    QRectF paddedBounds = bounds.adjusted(-padding, -padding, padding, padding);

    // 使视图适应填充后的边界
    fitInView(paddedBounds, Qt::KeepAspectRatio);
    currentZoom_ = transform().m11();

    // 更新 LOD
    updateLOD();
}

} // namespace nav
