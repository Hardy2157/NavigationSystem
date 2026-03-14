#include "core/graph/Graph.h"
#include <queue>
#include <unordered_set>

namespace nav {

const std::vector<Edge::Id> Graph::emptyEdgeList_;

Graph::Graph() : nextNodeId_(0), nextEdgeId_(0) {}

Node::Id Graph::addNode(const Point2D& position) {
    Node::Id id = nextNodeId_++;
    nodes_.emplace(id, Node(id, position));
    adjacencyList_[id] = std::vector<Edge::Id>();
    return id;
}

const Node* Graph::getNode(Node::Id id) const {
    auto it = nodes_.find(id);
    return (it != nodes_.end()) ? &it->second : nullptr;
}

Node* Graph::getNode(Node::Id id) {
    auto it = nodes_.find(id);
    return (it != nodes_.end()) ? &it->second : nullptr;
}

Edge::Id Graph::addEdge(Node::Id source, Node::Id target) {
    // Verify both nodes exist
    if (nodes_.find(source) == nodes_.end() || nodes_.find(target) == nodes_.end()) {
        return Edge::INVALID_ID;
    }

    // Calculate edge length based on node positions
    const Node* sourceNode = getNode(source);
    const Node* targetNode = getNode(target);
    double length = sourceNode->getPosition().distanceTo(targetNode->getPosition());

    // Create edge
    Edge::Id id = nextEdgeId_++;
    edges_.emplace(id, Edge(id, source, target, length));

    // Update adjacency list (undirected graph - add edge in both directions)
    adjacencyList_[source].push_back(id);
    adjacencyList_[target].push_back(id);

    return id;
}

const Edge* Graph::getEdge(Edge::Id id) const {
    auto it = edges_.find(id);
    return (it != edges_.end()) ? &it->second : nullptr;
}

Edge* Graph::getEdge(Edge::Id id) {
    auto it = edges_.find(id);
    return (it != edges_.end()) ? &it->second : nullptr;
}

const std::vector<Edge::Id>& Graph::getAdjacentEdges(Node::Id nodeId) const {
    auto it = adjacencyList_.find(nodeId);
    return (it != adjacencyList_.end()) ? it->second : emptyEdgeList_;
}

std::vector<Node::Id> Graph::getNeighbors(Node::Id nodeId) const {
    std::vector<Node::Id> neighbors;
    const auto& adjacentEdges = getAdjacentEdges(nodeId);

    for (Edge::Id edgeId : adjacentEdges) {
        const Edge* edge = getEdge(edgeId);
        if (edge) {
            // Add the other endpoint of the edge
            Node::Id neighbor = (edge->getSource() == nodeId) ? edge->getTarget() : edge->getSource();
            neighbors.push_back(neighbor);
        }
    }

    return neighbors;
}

bool Graph::isConnected() const {
    if (nodes_.empty()) {
        return true;
    }

    // Perform DFS from the first node
    std::unordered_map<Node::Id, bool> visited;
    for (const auto& pair : nodes_) {
        visited[pair.first] = false;
    }

    // Start DFS from first node
    Node::Id startNode = nodes_.begin()->first;
    dfsVisit(startNode, visited);

    // Check if all nodes were visited
    for (const auto& pair : visited) {
        if (!pair.second) {
            return false;
        }
    }

    return true;
}

void Graph::dfsVisit(Node::Id nodeId, std::unordered_map<Node::Id, bool>& visited) const {
    visited[nodeId] = true;

    const auto& adjacentEdges = getAdjacentEdges(nodeId);
    for (Edge::Id edgeId : adjacentEdges) {
        const Edge* edge = getEdge(edgeId);
        if (edge) {
            Node::Id neighbor = (edge->getSource() == nodeId) ? edge->getTarget() : edge->getSource();
            if (!visited[neighbor]) {
                dfsVisit(neighbor, visited);
            }
        }
    }
}

void Graph::clear() {
    nodes_.clear();
    edges_.clear();
    adjacencyList_.clear();
    nextNodeId_ = 0;
    nextEdgeId_ = 0;
}

} // namespace nav
