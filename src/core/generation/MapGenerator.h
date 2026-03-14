#ifndef MAPGENERATOR_H
#define MAPGENERATOR_H

#include "core/graph/Graph.h"

namespace nav {

// Abstract base class for map generation algorithms
class MapGenerator {
public:
    virtual ~MapGenerator() = default;

    // Generate a map with the specified number of nodes
    // @param graph The graph to populate with nodes and edges
    // @param numNodes Target number of nodes to generate
    // @param width Width of the map area
    // @param height Height of the map area
    virtual void generate(Graph& graph, int numNodes, double width, double height) = 0;
};

} // namespace nav

#endif // MAPGENERATOR_H
