#include "gui/view/MapScene.h"
#include "gui/items/NodeItem.h"
#include "gui/items/EdgeItem.h"
#include "gui/items/ClusterItem.h"
#include "core/traffic/TrafficModel.h"
#include "core/spatial/ClusterIndex.h"
#include <QGraphicsSceneMouseEvent>
#include <QPainterPath>
#include <QPen>
#include <iostream>

namespace nav {

namespace {

int edgeCongestionStatus(const Edge& edge) {
    return TrafficModel::getCongestionStatus(
        edge.getCapacity(),
        static_cast<double>(edge.getCarCount())
    );
}

int edgeDisplayStatus(const Edge& edge, bool heatmapVisible) {
    return heatmapVisible ? edgeCongestionStatus(edge) : 0;
}

} // namespace

MapScene::MapScene(QObject* parent)
    : QGraphicsScene(parent)
{
    // 设置浅灰色背景
    setBackgroundBrush(QBrush(QColor(245, 245, 245)));
}

void MapScene::loadMap(const Graph& graph) {
    // Clear existing items
    clearMap();

    // 禁用批量插入期间的索引以提高性能
    setItemIndexMethod(NoIndex);

    // 首先添加边（使其显示在节点下方）
    for (const auto& edgePair : graph.getEdges()) {
        const Edge& edge = edgePair.second;

        const Node* sourceNode = graph.getNode(edge.getSource());
        const Node* targetNode = graph.getNode(edge.getTarget());

        if (sourceNode && targetNode) {
            EdgeItem* item = new EdgeItem(
                edge.getId(),
                sourceNode->getPosition(),
                targetNode->getPosition(),
                edge.getRoadClass()
            );
            addItem(item);
            edgeItems_[edge.getId()] = item;
        }
    }

    // 添加节点
    for (const auto& nodePair : graph.getNodes()) {
        const Node& node = nodePair.second;

        NodeItem* item = new NodeItem(node.getId(), node.getPosition());
        addItem(item);
        nodeItems_[node.getId()] = item;
    }

    // 重新启用 BSP 树索引以进行高效查询
    setItemIndexMethod(BspTreeIndex);

    // 设置场景矩形以适应所有项目并留出一些填充
    QRectF bounds = itemsBoundingRect();
    double padding = 100.0;
    setSceneRect(bounds.adjusted(-padding, -padding, padding, padding));
}

void MapScene::clearMap() {
    clearPath();
    clear();
    nodeItems_.clear();
    edgeItems_.clear();
    highlightedNodes_.clear();
    highlightedEdges_.clear();
    trafficHighlightedEdges_.clear();
    clusterLevels_.clear();
    activeClusterLevel_ = -1;
    startNode_ = Node::INVALID_ID;
    endNode_ = Node::INVALID_ID;
    clickState_ = 0;
    pathItem_ = nullptr;
    queryPointMarker_ = nullptr;
    trafficPointMarker_ = nullptr;
}

void MapScene::resetAllEdgeStyles() {
    for (auto& pair : edgeItems_) {
        pair.second->applyBaseStyle();
    }
}

void MapScene::buildClusters(const Graph& graph, double mapWidth, double mapHeight) {
    // 清除旧聚类项
    for (auto& level : clusterLevels_) {
        for (ClusterItem* item : level) {
            removeItem(item);
            delete item;
        }
    }
    clusterLevels_.clear();
    activeClusterLevel_ = -1;

    // 构建多分辨率聚类索引
    double maxDim = std::max(mapWidth, mapHeight);
    std::vector<double> cellSizes = {
        maxDim / 15.0,   // Level 0: 粗粒度（低缩放）
        maxDim / 50.0    // Level 1: 细粒度（中缩放）
    };

    ClusterIndex index;
    index.buildMultiResolution(graph, mapWidth, mapHeight, cellSizes);

    // 为每个层级创建可视化项
    for (const auto& level : index.getLevels()) {
        std::vector<ClusterItem*> items;
        for (const auto& cluster : level.clusters) {
            ClusterItem* item = new ClusterItem(cluster.position, cluster.memberCount);
            item->setVisible(false);
            addItem(item);
            items.push_back(item);
        }
        clusterLevels_.push_back(std::move(items));
    }
}

void MapScene::setActiveClusterLevel(int level) {
    if (level == activeClusterLevel_) return;
    activeClusterLevel_ = level;

    for (int i = 0; i < static_cast<int>(clusterLevels_.size()); ++i) {
        bool visible = (i == level);
        for (ClusterItem* item : clusterLevels_[i]) {
            item->setVisible(visible);
        }
    }
}

NodeItem* MapScene::getNodeItem(Node::Id id) const {
    auto it = nodeItems_.find(id);
    return (it != nodeItems_.end()) ? it->second : nullptr;
}

EdgeItem* MapScene::getEdgeItem(Edge::Id id) const {
    auto it = edgeItems_.find(id);
    return (it != edgeItems_.end()) ? it->second : nullptr;
}

void MapScene::updateEdgeCongestion(Edge::Id id, int congestionStatus) {
    if (highlightedEdges_.count(id)) {
        return;
    }

    EdgeItem* item = getEdgeItem(id);
    if (item) {
        item->updateStyle(congestionStatus);
    }
}

void MapScene::clearPathSelection() {
    clearPath();

    // 重置节点颜色和高亮状态
    if (startNode_ != Node::INVALID_ID) {
        NodeItem* item = getNodeItem(startNode_);
        if (item) {
            item->setBrush(QBrush(QColor(60, 60, 60)));  // 原始深灰色
            item->setPen(Qt::NoPen);
            item->setHighlighted(false);  // 重置为小尺寸
        }
    }
    if (endNode_ != Node::INVALID_ID) {
        NodeItem* item = getNodeItem(endNode_);
        if (item) {
            item->setBrush(QBrush(QColor(60, 60, 60)));  // 原始深灰色
            item->setPen(Qt::NoPen);
            item->setHighlighted(false);  // 重置为小尺寸
        }
    }

    startNode_ = Node::INVALID_ID;
    endNode_ = Node::INVALID_ID;
    clickState_ = 0;
}

void MapScene::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() != Qt::LeftButton) {
        QGraphicsScene::mousePressEvent(event);
        return;
    }

    // 查找点击的节点
    QList<QGraphicsItem*> clickedItems = items(event->scenePos());
    NodeItem* clickedNode = nullptr;

    for (QGraphicsItem* item : clickedItems) {
        clickedNode = dynamic_cast<NodeItem*>(item);
        if (clickedNode) break;
    }

    if (!clickedNode) {
        QGraphicsScene::mousePressEvent(event);
        return;
    }

    Node::Id clickedId = clickedNode->getNodeId();

    switch (clickState_) {
        case 0:  // 第一次点击：设置起点
            clearPathSelection();
            startNode_ = clickedId;
            highlightNode(startNode_, QColor(33, 150, 243));  // 蓝色
            clickState_ = 1;
            emit statusMessage(QString("起点: 节点 %1。请点击另一个节点作为终点。").arg(startNode_));
            break;

        case 1:  // 第二次点击：设置终点并查找路径
            if (clickedId == startNode_) {
                // 点击了相同节点，忽略
                break;
            }
            endNode_ = clickedId;
            highlightNode(endNode_, QColor(156, 39, 176));  // 紫色

            // 查找路径
            if (graph_ && pathfinder_) {
                PathResult result = pathfinder_->findPath(*graph_, startNode_, endNode_);
                if (result.found) {
                    showPath(result);
                    emit pathFound(result);
                    emit statusMessage(QString("路径已找到: %1 个节点, 开销: %2")
                                           .arg(result.pathNodes.size())
                                           .arg(result.totalCost, 0, 'f', 2));
                } else {
                    emit errorOccurred("路径查找", "未找到路径！");
                }
            } else {
                emit errorOccurred("路径查找", "路径查找器未配置！请先生成或加载地图。");
            }
            clickState_ = 2;
            break;

        case 2:  // 第三次点击：清除并重新开始
            clearPathSelection();
            startNode_ = clickedId;
            highlightNode(startNode_, QColor(33, 150, 243));  // 蓝色
            clickState_ = 1;
            emit statusMessage(QString("起点: 节点 %1。请点击另一个节点作为终点。").arg(startNode_));
            break;
    }

    QGraphicsScene::mousePressEvent(event);
}

void MapScene::highlightNode(Node::Id id, const QColor& color) {
    NodeItem* item = getNodeItem(id);
    if (item) {
        item->setBrush(QBrush(color));
        item->setPen(QPen(color.darker(130), 2));  // 添加边框
        item->setHighlighted(true);  // 使其在视觉上更大（16px 直径）
        item->setZValue(12.0);  // 置于前面
    }
}

void MapScene::showPath(const PathResult& result) {
    clearPath();

    if (!graph_ || result.pathNodes.size() < 2) return;

    // 创建路径
    QPainterPath path;

    const Node* firstNode = graph_->getNode(result.pathNodes[0]);
    if (firstNode) {
        path.moveTo(firstNode->getPosition().x, firstNode->getPosition().y);
    }

    for (size_t i = 1; i < result.pathNodes.size(); ++i) {
        const Node* node = graph_->getNode(result.pathNodes[i]);
        if (node) {
            path.lineTo(node->getPosition().x, node->getPosition().y);
        }
    }

    // 创建路径项
    pathItem_ = new QGraphicsPathItem(path);
    QPen pen(QColor(33, 150, 243, 220));  // 半透明蓝色
    pen.setWidthF(6.0);  // 更粗的路径以提高可见性
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    pathItem_->setPen(pen);
    pathItem_->setZValue(5.0);  // 在边之上，在高亮节点之下

    addItem(pathItem_);
}

void MapScene::clearPath() {
    if (pathItem_) {
        removeItem(pathItem_);
        delete pathItem_;
        pathItem_ = nullptr;
    }
}

void MapScene::highlightNodes(const std::vector<Node::Id>& nodeIds, const QColor& color) {
    // Clear previous highlights first
    clearSpatialHighlights();

    highlightedNodes_ = nodeIds;

    for (Node::Id id : nodeIds) {
        NodeItem* item = getNodeItem(id);
        if (item) {
            item->setBrush(QBrush(color));
            item->setPen(QPen(color.darker(120), 2));  // 添加边框以提高可见性
            item->setHighlighted(true);  // 使其在视觉上更大（16px 直径）
            item->setZValue(8.0);  // 在普通节点之上
        }
    }
}

void MapScene::highlightEdges(const std::vector<Edge::Id>& edgeIds, const QColor& color) {
    highlightedEdges_.insert(edgeIds.begin(), edgeIds.end());

    QPen pen(color);
    pen.setWidthF(3.0);
    pen.setCapStyle(Qt::RoundCap);

    for (Edge::Id id : edgeIds) {
        EdgeItem* item = getEdgeItem(id);
        if (item) {
            item->setPen(pen);
            item->setZValue(2.0);  // 在普通边之上
        }
    }
}

void MapScene::clearSpatialHighlights() {
    // 将之前高亮的节点重置为默认状态
    for (Node::Id id : highlightedNodes_) {
        NodeItem* item = getNodeItem(id);
        if (item) {
            item->setBrush(QBrush(QColor(60, 60, 60)));  // 原始深灰色
            item->setPen(Qt::NoPen);  // 移除边框
            item->setHighlighted(false);  // 重置为小尺寸
            item->setZValue(10.0);  // 原始 Z 值
        }
    }
    highlightedNodes_.clear();

    // 将之前高亮的边恢复到正确状态
    for (Edge::Id id : highlightedEdges_) {
        EdgeItem* item = getEdgeItem(id);
        if (item) {
            int status = 0;  // 热力图关闭时默认为绿色
            if (heatmapVisible_ && graph_) {
                const Edge* edge = graph_->getEdge(id);
                if (edge) {
                    status = edgeDisplayStatus(*edge, heatmapVisible_);
                }
            }
            item->updateStyle(status);
        }
    }
    highlightedEdges_.clear();

    // 移除查询点标记
    if (queryPointMarker_) {
        removeItem(queryPointMarker_);
        delete queryPointMarker_;
        queryPointMarker_ = nullptr;
    }
}

PathResult MapScene::findPathById(Node::Id startId, Node::Id endId) {
    PathResult result;

    if (!graph_ || !pathfinder_) {
        result.found = false;
        emit errorOccurred("路径查找", "路径查找器未配置！请先生成或加载地图。");
        return result;
    }

    // 清除之前的路径选择
    clearPathSelection();

    // 设置起点和终点节点
    startNode_ = startId;
    endNode_ = endId;

    // 高亮显示起点和终点节点
    highlightNode(startNode_, QColor(33, 150, 243));  // 蓝色
    highlightNode(endNode_, QColor(156, 39, 176));    // 紫色

    // 查找路径
    result = pathfinder_->findPath(*graph_, startNode_, endNode_);

    if (result.found) {
        showPath(result);
        emit pathFound(result);
        emit statusMessage(QString("路径已找到: %1 个节点, 开销: %2")
                               .arg(result.pathNodes.size())
                               .arg(result.totalCost, 0, 'f', 2));
    } else {
        emit errorOccurred("路径查找", QString("未找到从节点 %1 到节点 %2 的路径！")
                               .arg(startId).arg(endId));
    }

    clickState_ = 2;  // 路径已显示状态
    return result;
}

void MapScene::showQueryPoint(double x, double y) {
    placeMarker(queryPointMarker_, x, y, QColor(255, 0, 0, 150), QColor(200, 0, 0));
}

void MapScene::showTrafficPoint(double x, double y) {
    placeMarker(trafficPointMarker_, x, y, QColor(255, 87, 34, 150), QColor(230, 74, 25));
}

void MapScene::placeMarker(QGraphicsEllipseItem*& marker, double x, double y,
                            const QColor& fill, const QColor& border) {
    if (marker) {
        removeItem(marker);
        delete marker;
    }
    marker = new QGraphicsEllipseItem(x - 8, y - 8, 16, 16);
    marker->setBrush(QBrush(fill));
    marker->setPen(QPen(border, 2));
    marker->setZValue(15.0);
    addItem(marker);
}

void MapScene::showTrafficEdges(const std::vector<Edge::Id>& edgeIds, const Graph& graph) {
    clearTrafficHighlights();
    trafficHighlightedEdges_ = edgeIds;

    for (Edge::Id id : edgeIds) {
        EdgeItem* item = getEdgeItem(id);
        const Edge* edge = graph.getEdge(id);
        if (item && edge) {
            int status = edgeDisplayStatus(*edge, heatmapVisible_);
            item->updateStyle(status);
            item->setZValue(2.0);  // Above normal edges
        }
    }
}

void MapScene::clearTrafficHighlights() {
    // 根据热力图可见性将边恢复到正确状态
    for (Edge::Id id : trafficHighlightedEdges_) {
        EdgeItem* item = getEdgeItem(id);
        if (item) {
            int status = 0;  // 热力图关闭时默认为绿色
            if (heatmapVisible_ && graph_) {
                const Edge* edge = graph_->getEdge(id);
                if (edge) {
                    status = edgeDisplayStatus(*edge, heatmapVisible_);
                }
            }
            item->updateStyle(status);
        }
    }
    trafficHighlightedEdges_.clear();

    if (trafficPointMarker_) {
        removeItem(trafficPointMarker_);
        delete trafficPointMarker_;
        trafficPointMarker_ = nullptr;
    }
}

void MapScene::updateTrafficHighlights(const Graph& graph) {
    for (Edge::Id id : trafficHighlightedEdges_) {
        EdgeItem* item = getEdgeItem(id);
        const Edge* edge = graph.getEdge(id);
        if (item && edge) {
            int status = edgeDisplayStatus(*edge, heatmapVisible_);
            item->updateStyle(status);
        }
    }
}

} // namespace nav
