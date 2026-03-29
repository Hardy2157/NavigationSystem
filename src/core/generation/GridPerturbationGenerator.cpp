#include "core/generation/GridPerturbationGenerator.h"
#include <cmath>
#include <iostream>
#include <algorithm>

namespace nav {

// ============================================================================
// 用于 Kruskal 最小生成树算法的并查集（不相交集合）
// ============================================================================
namespace {

class UnionFind {
public:
    explicit UnionFind(size_t n) : parent_(n), rank_(n, 0) {
        for (size_t i = 0; i < n; ++i) {
            parent_[i] = i;
        }
    }

    size_t find(size_t x) {
        if (parent_[x] != x) {
            parent_[x] = find(parent_[x]);  // 路径压缩
        }
        return parent_[x];
    }

    bool unite(size_t x, size_t y) {
        size_t rootX = find(x);
        size_t rootY = find(y);
        if (rootX == rootY) {
            return false;  // 已在同一集合中
        }
        // 按秩合并
        if (rank_[rootX] < rank_[rootY]) {
            parent_[rootX] = rootY;
        } else if (rank_[rootX] > rank_[rootY]) {
            parent_[rootY] = rootX;
        } else {
            parent_[rootY] = rootX;
            rank_[rootX]++;
        }
        return true;
    }

private:
    std::vector<size_t> parent_;
    std::vector<size_t> rank_;
};

// 用于 MST 计算的边结构
struct TempEdge {
    Node::Id source;
    Node::Id target;
    double weight;  // MST 随机化的随机权重
};

} // anonymous namespace

GridPerturbationGenerator::GridPerturbationGenerator()
    : rng_(std::random_device{}())
    , perturbationFactor_(0.4)
    , addDiagonals_(true)
    , diagonalProbability_(0.5)
{}

GridPerturbationGenerator::GridPerturbationGenerator(unsigned int seed)
    : rng_(seed)
    , perturbationFactor_(0.4)
    , addDiagonals_(true)
    , diagonalProbability_(0.5)
{}

void GridPerturbationGenerator::generate(Graph& graph, int numNodes, double width, double height) {
    // 清除现有图
    graph.clear();

    if (numNodes <= 0) {
        return;
    }

    // 计算网格维度（近似正方形）
    int cols = static_cast<int>(std::ceil(std::sqrt(numNodes * width / height)));
    int rows = static_cast<int>(std::ceil(static_cast<double>(numNodes) / cols));

    // 如果单元格过多则调整
    while (rows * cols > numNodes * 1.5) {
        if (cols > rows) {
            cols--;
        } else {
            rows--;
        }
    }

    std::cout << "Generating " << numNodes << " nodes in a " << rows << "x" << cols
              << " grid (" << rows * cols << " cells)" << std::endl;

    // 计算单元格维度
    double cellWidth = width / cols;
    double cellHeight = height / rows;

    // 创建扰动分布
    std::uniform_real_distribution<double> perturbDist(-perturbationFactor_, perturbationFactor_);

    // 将节点 ID 存储在 2D 网格中以便于邻居查找
    std::vector<std::vector<Node::Id>> nodeGrid(rows, std::vector<Node::Id>(cols, Node::INVALID_ID));

    // 生成带扰动的节点
    int nodesCreated = 0;
    for (int i = 0; i < rows && nodesCreated < numNodes; ++i) {
        for (int j = 0; j < cols && nodesCreated < numNodes; ++j) {
            // 计算基础位置（单元格中心）
            double baseX = (j + 0.5) * cellWidth;
            double baseY = (i + 0.5) * cellHeight;

            // 添加随机扰动
            double offsetX = perturbDist(rng_) * cellWidth;
            double offsetY = perturbDist(rng_) * cellHeight;

            // 限制在单元格边界内（带小边距）
            double x = std::max(j * cellWidth + cellWidth * 0.05,
                              std::min(baseX + offsetX, (j + 1) * cellWidth - cellWidth * 0.05));
            double y = std::max(i * cellHeight + cellHeight * 0.05,
                              std::min(baseY + offsetY, (i + 1) * cellHeight - cellHeight * 0.05));

            // 创建节点
            Node::Id nodeId = graph.addNode(Point2D(x, y));
            nodeGrid[i][j] = nodeId;
            nodesCreated++;
        }
    }

    std::cout << "Created " << nodesCreated << " nodes" << std::endl;

    // ========================================================================
    // 步骤 1：收集所有潜在边（网格连接 + 对角线）
    // ========================================================================
    std::vector<TempEdge> allEdges;
    std::uniform_real_distribution<double> weightDist(0.0, 1.0);
    std::uniform_real_distribution<double> diagChance(0.0, 1.0);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            Node::Id currentNode = nodeGrid[i][j];
            if (currentNode == Node::INVALID_ID) {
                continue;
            }

            // 右邻居（水平边）
            if (j + 1 < cols && nodeGrid[i][j + 1] != Node::INVALID_ID) {
                allEdges.push_back({currentNode, nodeGrid[i][j + 1], weightDist(rng_)});
            }

            // 下邻居（垂直边）
            if (i + 1 < rows && nodeGrid[i + 1][j] != Node::INVALID_ID) {
                allEdges.push_back({currentNode, nodeGrid[i + 1][j], weightDist(rng_)});
            }

            // 对角线边（棋盘模式防止 X 交叉）
            if (addDiagonals_ && (i + j) % 2 == 0) {
                // 右下对角线
                if (i + 1 < rows && j + 1 < cols && nodeGrid[i + 1][j + 1] != Node::INVALID_ID) {
                    if (diagChance(rng_) < diagonalProbability_) {
                        allEdges.push_back({currentNode, nodeGrid[i + 1][j + 1], weightDist(rng_)});
                    }
                }

                // 左下对角线
                if (i + 1 < rows && j - 1 >= 0 && nodeGrid[i + 1][j - 1] != Node::INVALID_ID) {
                    if (diagChance(rng_) < diagonalProbability_) {
                        allEdges.push_back({currentNode, nodeGrid[i + 1][j - 1], weightDist(rng_)});
                    }
                }
            }
        }
    }

    std::cout << "Collected " << allEdges.size() << " potential edges" << std::endl;

    // ========================================================================
    // 步骤 2：运行 Kruskal 算法查找 MST
    // ========================================================================
    // 按随机权重排序边
    std::sort(allEdges.begin(), allEdges.end(),
              [](const TempEdge& a, const TempEdge& b) { return a.weight < b.weight; });

    UnionFind uf(static_cast<size_t>(nodesCreated));
    std::vector<TempEdge> mstEdges;
    std::vector<TempEdge> nonMstEdges;

    for (const auto& edge : allEdges) {
        if (uf.unite(edge.source, edge.target)) {
            mstEdges.push_back(edge);
        } else {
            nonMstEdges.push_back(edge);
        }
    }

    std::cout << "MST edges: " << mstEdges.size()
              << ", Non-MST edges: " << nonMstEdges.size() << std::endl;

    // ========================================================================
    // 步骤 3：保留 MST + 60% 的非 MST 边（移除 40%）
    // ========================================================================
    const double keepRatio = 0.6;  // 保留 60% 的非 MST 边以获得合理密度
    size_t nonMstToKeep = static_cast<size_t>(nonMstEdges.size() * keepRatio);

    // 打乱非 MST 边并仅保留一部分
    std::shuffle(nonMstEdges.begin(), nonMstEdges.end(), rng_);
    nonMstEdges.resize(nonMstToKeep);

    // ========================================================================
    // 步骤 4：将最终边添加到图中并设置可变容量
    // ========================================================================
    int edgesCreated = 0;
    double avgCellDim = (cellWidth + cellHeight) / 2.0;
    std::uniform_real_distribution<double> capacityNoise(-2.0, 2.0);

    auto setEdgeCapacity = [&](Edge::Id edgeId) {
        Edge* e = graph.getEdge(edgeId);
        if (e) {
            double lengthRatio = e->getLength() / avgCellDim;
            double capacity = 5.0 + 10.0 * lengthRatio + capacityNoise(rng_);
            e->setCapacity(std::max(3.0, capacity));
        }
    };

    // 添加所有 MST 边（保证连通性）
    for (const auto& edge : mstEdges) {
        Edge::Id eid = graph.addEdge(edge.source, edge.target);
        setEdgeCapacity(eid);
        edgesCreated++;
    }

    // 添加保留的非 MST 边（提供替代路径）
    for (const auto& edge : nonMstEdges) {
        Edge::Id eid = graph.addEdge(edge.source, edge.target);
        setEdgeCapacity(eid);
        edgesCreated++;
    }

    std::cout << "Final graph: " << edgesCreated << " edges (sparsified from "
              << allEdges.size() << ")" << std::endl;
}

} // namespace nav
