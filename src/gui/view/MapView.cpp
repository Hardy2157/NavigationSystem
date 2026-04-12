#include "gui/view/MapView.h"
#include "gui/view/MapScene.h"
#include "gui/items/NodeItem.h"
#include "gui/items/EdgeItem.h"
#include <QWheelEvent>
#include <QScrollBar>
#include <unordered_set>

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
    const ZoomBand& band = displayFilter_.getBand(scale);

    // 获取高亮边集合，用于确保交互边始终可见
    const auto& highlightedEdges = mapScene->getHighlightedEdges();
    const auto& trafficEdgesVec = mapScene->getTrafficHighlightedEdges();
    std::unordered_set<Edge::Id> trafficEdgeSet(trafficEdgesVec.begin(), trafficEdgesVec.end());

    // 根据道路等级和缩放带设置边可见性
    for (const auto& pair : mapScene->getEdgeItems()) {
        EdgeItem* item = pair.second;
        if (!item) continue;

        // 高亮边和交通边始终可见
        Edge::Id edgeId = item->getEdgeId();
        if (highlightedEdges.count(edgeId) > 0 || trafficEdgeSet.count(edgeId) > 0) {
            item->setVisible(true);
            continue;
        }

        bool visible;
        switch (item->getRoadClass()) {
            case RoadClass::Arterial:
                visible = band.showArterialEdges;
                break;
            case RoadClass::Secondary:
                visible = band.showSecondaryEdges;
                break;
            case RoadClass::Local:
            default:
                visible = band.showLocalEdges;
                break;
        }
        item->setVisible(visible);
    }

    // 根据缩放带设置节点可见性（交互节点始终可见）
    for (const auto& pair : mapScene->getNodeItems()) {
        NodeItem* item = pair.second;
        if (!item) continue;

        if (item->isInteractive()) {
            item->setVisible(true);
        } else {
            item->setVisible(band.showNormalNodes);
        }
    }

    // 设置聚类层级
    mapScene->setActiveClusterLevel(band.clusterLevel);
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
