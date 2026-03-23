#ifndef GRAPHIO_H
#define GRAPHIO_H

#include "core/graph/Graph.h"
#include <string>

namespace nav {

struct MapFileData {
    Graph graph;
    double width = 0.0;
    double height = 0.0;
};

class GraphIO {
public:
    // Save graph to file. Returns true on success.
    static bool save(const std::string& filepath, const Graph& graph,
                     double mapWidth, double mapHeight);

    // Load graph from file. Returns true on success, populates outData.
    static bool load(const std::string& filepath, MapFileData& outData);

    // Get last error message (for UI display)
    static const std::string& getLastError();

private:
    static std::string lastError_;
};

} // namespace nav

#endif // GRAPHIO_H
