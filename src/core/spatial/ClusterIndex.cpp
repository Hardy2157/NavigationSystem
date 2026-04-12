#include "core/spatial/ClusterIndex.h"
#include <unordered_map>
#include <cmath>

namespace nav {

void ClusterIndex::buildSingleLevel(const Graph& graph, double mapWidth,
                                     double gridCellSize, std::vector<Cluster>& out) {
    out.clear();
    if (gridCellSize <= 0.0 || graph.getNodeCount() == 0) return;

    int numCols = std::max(1, static_cast<int>(std::ceil(mapWidth / gridCellSize)));

    struct CellData {
        double sumX = 0.0;
        double sumY = 0.0;
        size_t count = 0;
    };
    std::unordered_map<uint64_t, CellData> cells;

    for (const auto& pair : graph.getNodes()) {
        const Node& node = pair.second;
        const Point2D& pos = node.getPosition();

        int col = static_cast<int>(pos.x / gridCellSize);
        int row = static_cast<int>(pos.y / gridCellSize);
        uint64_t key = static_cast<uint64_t>(row) * numCols + col;

        auto& cell = cells[key];
        cell.sumX += pos.x;
        cell.sumY += pos.y;
        cell.count++;
    }

    out.reserve(cells.size());
    for (const auto& pair : cells) {
        const CellData& cell = pair.second;
        if (cell.count > 0) {
            Cluster cluster;
            cluster.position = Point2D(cell.sumX / cell.count,
                                        cell.sumY / cell.count);
            cluster.memberCount = cell.count;
            out.push_back(cluster);
        }
    }
}

void ClusterIndex::build(const Graph& graph, double mapWidth,
                          double mapHeight, double gridCellSize) {
    gridCellSize_ = gridCellSize;
    buildSingleLevel(graph, mapWidth, gridCellSize, clusters_);
}

void ClusterIndex::buildMultiResolution(const Graph& graph, double mapWidth,
                                         double mapHeight,
                                         const std::vector<double>& cellSizes) {
    levels_.clear();
    levels_.reserve(cellSizes.size());

    for (double cellSize : cellSizes) {
        ClusterLevel level;
        level.cellSize = cellSize;
        buildSingleLevel(graph, mapWidth, cellSize, level.clusters);
        levels_.push_back(std::move(level));
    }
}

} // namespace nav
