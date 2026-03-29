#ifndef GRIDPERTURBATIONGENERATOR_H
#define GRIDPERTURBATIONGENERATOR_H

#include "core/generation/MapGenerator.h"
#include <random>
#include <vector>

namespace nav {

// 使用基于网格的方法和随机扰动的地图生成器
// 通过在网格模式中放置节点并添加随机偏移，然后连接相邻网格单元来创建平面连通图
class GridPerturbationGenerator : public MapGenerator {
public:
    GridPerturbationGenerator();
    explicit GridPerturbationGenerator(unsigned int seed);

    void generate(Graph& graph, int numNodes, double width, double height) override;

    // 配置
    void setPerturbationFactor(double factor) { perturbationFactor_ = factor; }
    void setAddDiagonals(bool addDiagonals) { addDiagonals_ = addDiagonals; }
    void setDiagonalProbability(double prob) { diagonalProbability_ = prob; }

private:
    std::mt19937 rng_;
    double perturbationFactor_;  // 从网格中心扰动的程度 (0.0 到 0.5)
    bool addDiagonals_;           // 是否添加对角线连接
    double diagonalProbability_;  // 添加每条对角线边的概率
};

} // namespace nav

#endif // GRIDPERTURBATIONGENERATOR_H
