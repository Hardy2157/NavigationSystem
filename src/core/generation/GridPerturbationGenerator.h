#ifndef GRIDPERTURBATIONGENERATOR_H
#define GRIDPERTURBATIONGENERATOR_H

#include "core/generation/MapGenerator.h"
#include <random>
#include <vector>

namespace nav {

// Map generator using grid-based approach with random perturbation
// Creates a planar, connected graph by placing nodes in a grid pattern
// with random offsets, then connecting adjacent grid cells
class GridPerturbationGenerator : public MapGenerator {
public:
    GridPerturbationGenerator();
    explicit GridPerturbationGenerator(unsigned int seed);

    void generate(Graph& graph, int numNodes, double width, double height) override;

    // Configuration
    void setPerturbationFactor(double factor) { perturbationFactor_ = factor; }
    void setAddDiagonals(bool addDiagonals) { addDiagonals_ = addDiagonals; }
    void setDiagonalProbability(double prob) { diagonalProbability_ = prob; }

private:
    std::mt19937 rng_;
    double perturbationFactor_;  // How much to perturb from grid center (0.0 to 0.5)
    bool addDiagonals_;           // Whether to add diagonal connections
    double diagonalProbability_;  // Probability of adding each diagonal edge
};

} // namespace nav

#endif // GRIDPERTURBATIONGENERATOR_H
