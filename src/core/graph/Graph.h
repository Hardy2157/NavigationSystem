#ifndef GRAPH_H
#define GRAPH_H

#include "core/graph/Node.h"
#include "core/graph/Edge.h"
#include <vector>
#include <unordered_map>
#include <memory>

namespace nav {

// 图类，使用邻接表表示
class Graph {
public:
    Graph();

    // 节点操作
    Node::Id addNode(const Point2D& position);
    Node::Id addNodeWithId(Node::Id id, const Point2D& position);
    const Node* getNode(Node::Id id) const;
    Node* getNode(Node::Id id);
    size_t getNodeCount() const { return nodes_.size(); }
    const std::unordered_map<Node::Id, Node>& getNodes() const { return nodes_; }

    // 边操作
    Edge::Id addEdge(Node::Id source, Node::Id target);
    Edge::Id addEdgeWithId(Edge::Id id, Node::Id source, Node::Id target,
                           double length, double capacity);
    const Edge* getEdge(Edge::Id id) const;
    Edge* getEdge(Edge::Id id);
    size_t getEdgeCount() const { return edges_.size(); }
    const std::unordered_map<Edge::Id, Edge>& getEdges() const { return edges_; }

    // 邻接操作
    const std::vector<Edge::Id>& getAdjacentEdges(Node::Id nodeId) const;
    std::vector<Node::Id> getNeighbors(Node::Id nodeId) const;

    // 图属性
    bool isConnected() const;
    void clear();
    void reserve(size_t nodeCount, size_t edgeCount);

private:
    // 连通性检查辅助函数 (DFS)
    void dfsVisit(Node::Id nodeId, std::unordered_map<Node::Id, bool>& visited) const;

    std::unordered_map<Node::Id, Node> nodes_;
    std::unordered_map<Edge::Id, Edge> edges_;
    std::unordered_map<Node::Id, std::vector<Edge::Id>> adjacencyList_;

    Node::Id nextNodeId_;
    Edge::Id nextEdgeId_;

    static const std::vector<Edge::Id> emptyEdgeList_;
};

} // namespace nav

#endif // GRAPH_H
