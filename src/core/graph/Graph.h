#ifndef GRAPH_H
#define GRAPH_H

#include "core/graph/Node.h"
#include "core/graph/Edge.h"
#include <vector>
#include <unordered_map>
#include <memory>

namespace nav {

// Graph class using adjacency list representation
class Graph {
public:
    Graph();

    // Node operations
    Node::Id addNode(const Point2D& position);
    const Node* getNode(Node::Id id) const;
    Node* getNode(Node::Id id);
    size_t getNodeCount() const { return nodes_.size(); }
    const std::unordered_map<Node::Id, Node>& getNodes() const { return nodes_; }

    // Edge operations
    Edge::Id addEdge(Node::Id source, Node::Id target);
    const Edge* getEdge(Edge::Id id) const;
    Edge* getEdge(Edge::Id id);
    size_t getEdgeCount() const { return edges_.size(); }
    const std::unordered_map<Edge::Id, Edge>& getEdges() const { return edges_; }

    // Adjacency operations
    const std::vector<Edge::Id>& getAdjacentEdges(Node::Id nodeId) const;
    std::vector<Node::Id> getNeighbors(Node::Id nodeId) const;

    // Graph properties
    bool isConnected() const;
    void clear();

private:
    // Helper for connectivity check (DFS)
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
