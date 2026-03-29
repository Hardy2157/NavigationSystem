#ifndef TRAFFICSIMULATOR_H
#define TRAFFICSIMULATOR_H

#include "core/graph/Graph.h"
#include "core/traffic/TrafficModel.h"
#include "core/algorithms/PathFinder.h"
#include <random>
#include <vector>

namespace nav {

class TrafficSimulator {
public:
    // 沿计算路线行驶的车辆代理
    struct Car {
        Edge::Id currentEdge;       // 此车当前所在的道路
        double remainingTime;       // 离开此道路前的剩余时间
        std::vector<Edge::Id> route; // 计划路线（边序列）
        size_t routeIndex;          // 路线中的当前位置
    };

    TrafficSimulator(Graph& graph, PathFinder* pathfinder, unsigned int seed = 42);

    // 执行一个仿真步骤（推进所有车辆，生成新车辆）
    void step();

    // 获取上一步中更改的边列表
    const std::vector<Edge::Id>& getChangedEdges() const { return changedEdges_; }

    // 获取图引用以进行可视化更新
    Graph& getGraph() { return graph_; }

    // 获取活动车辆数
    size_t getActiveCarCount() const { return activeCars_.size(); }

    // 将所有交通重置为零
    void reset();

    // 配置
    void setSpawnRate(int carsPerStep) { spawnRate_ = carsPerStep; }
    void setMaxCars(int maxCars) { maxCars_ = maxCars; }
    void setTimeStep(double dt) { timeStep_ = dt; }

private:
    void spawnCars();
    void advanceCars();
    double computeTravelTime(const Edge* edge) const;

    Graph& graph_;
    PathFinder* pathfinder_;
    std::mt19937 rng_;

    std::vector<Car> activeCars_;
    std::vector<Edge::Id> changedEdges_;
    std::vector<Node::Id> allNodeIds_;

    // 仿真参数
    int spawnRate_ = 10;     // 每步生成的车辆数
    int maxCars_ = 1000;     // 最大活动车辆数
    double timeStep_ = 1.0;  // 每步的时间单位
    int stepCount_ = 0;
};

} // namespace nav

#endif // TRAFFICSIMULATOR_H
