#ifndef MAPSCENE_H
#define MAPSCENE_H

#include <QGraphicsScene>
#include <QGraphicsPathItem>
#include <QGraphicsEllipseItem>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "core/graph/Graph.h"
#include "core/graph/Node.h"
#include "core/graph/Edge.h"
#include "core/algorithms/PathFinder.h"
#include "gui/items/ClusterItem.h"

namespace nav {

class NodeItem;
class EdgeItem;

class MapScene : public QGraphicsScene {
    Q_OBJECT

public:
    explicit MapScene(QObject* parent = nullptr);

    // 将图加载到场景中
    void loadMap(const Graph& graph);

    // 清除所有地图项
    void clearMap();

    // 通过 ID 获取项以进行更新
    NodeItem* getNodeItem(Node::Id id) const;
    EdgeItem* getEdgeItem(Edge::Id id) const;

    // 根据拥堵情况更新边样式
    void updateEdgeCongestion(Edge::Id id, int congestionStatus);

    // 设置用于路径查找的图引用
    void setGraph(Graph* graph) { graph_ = graph; }
    Graph* getGraph() const { return graph_; }

    // 设置要使用的路径查找器
    void setPathFinder(PathFinder* pathfinder) { pathfinder_ = pathfinder; }

    // 热力图可见性（由 MainWindow 保持同步）
    void setHeatmapVisible(bool visible) { heatmapVisible_ = visible; }
    bool isHeatmapVisible() const { return heatmapVisible_; }

    // 清除当前路径选择
    void clearPathSelection();

    // 获取所有节点项以进行 LOD 更新
    const std::unordered_map<Node::Id, NodeItem*>& getNodeItems() const { return nodeItems_; }

    // 获取所有边项以进行 LOD 更新
    const std::unordered_map<Edge::Id, EdgeItem*>& getEdgeItems() const { return edgeItems_; }

    // 将所有边恢复为道路等级默认样式
    void resetAllEdgeStyles();

    // 聚类可视化
    void buildClusters(const Graph& graph, double mapWidth, double mapHeight);
    void setActiveClusterLevel(int level);

    // 高亮显示节点列表（用于空间查询结果）
    void highlightNodes(const std::vector<Node::Id>& nodeIds, const QColor& color);

    // 高亮显示边列表（用于空间查询结果）
    void highlightEdges(const std::vector<Edge::Id>& edgeIds, const QColor& color);

    // 清除空间查询高亮
    void clearSpatialHighlights();

    // 通过显式节点 ID 查找路径（用于控制面板）
    PathResult findPathById(Node::Id startId, Node::Id endId);

    // 显示查询点标记
    void showQueryPoint(double x, double y);
    void showTrafficPoint(double x, double y);

    // 局部视图的交通可视化 (F4)
    void showTrafficEdges(const std::vector<Edge::Id>& edgeIds, const Graph& graph);
    void clearTrafficHighlights();
    void updateTrafficHighlights(const Graph& graph);
    const std::vector<Edge::Id>& getTrafficHighlightedEdges() const { return trafficHighlightedEdges_; }
    const std::unordered_set<Edge::Id>& getHighlightedEdges() const { return highlightedEdges_; }

signals:
    void pathFound(const PathResult& result);
    void statusMessage(const QString& message);
    void errorOccurred(const QString& title, const QString& message);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

private:
    void highlightNode(Node::Id id, const QColor& color);
    void showPath(const PathResult& result);
    void clearPath();
    void placeMarker(QGraphicsEllipseItem*& marker, double x, double y,
                     const QColor& fill, const QColor& border);

    std::unordered_map<Node::Id, NodeItem*> nodeItems_;
    std::unordered_map<Edge::Id, EdgeItem*> edgeItems_;

    // 路径查找状态
    Graph* graph_ = nullptr;
    PathFinder* pathfinder_ = nullptr;
    bool heatmapVisible_ = false;
    Node::Id startNode_ = Node::INVALID_ID;
    Node::Id endNode_ = Node::INVALID_ID;
    int clickState_ = 0;  // 0: 等待起点, 1: 等待终点, 2: 路径已显示

    // 路径可视化
    QGraphicsPathItem* pathItem_ = nullptr;
    QColor originalStartColor_;
    QColor originalEndColor_;

    // 空间查询可视化
    std::vector<Node::Id> highlightedNodes_;
    std::unordered_set<Edge::Id> highlightedEdges_;
    QGraphicsEllipseItem* queryPointMarker_ = nullptr;

    // 交通局部视图 (F4)
    std::vector<Edge::Id> trafficHighlightedEdges_;
    QGraphicsEllipseItem* trafficPointMarker_ = nullptr;

    // 聚类可视化（多分辨率）
    std::vector<std::vector<ClusterItem*>> clusterLevels_;
    int activeClusterLevel_ = -1;
};

} // namespace nav

#endif // MAPSCENE_H
