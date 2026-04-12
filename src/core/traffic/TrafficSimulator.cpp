#include "core/traffic/TrafficSimulator.h"
#include <algorithm>
#include <iostream>

namespace nav {

TrafficSimulator::TrafficSimulator(Graph& graph, PathFinder* pathfinder, unsigned int seed)
    : graph_(graph)
    , pathfinder_(pathfinder)
    , rng_(seed)
{
    // 缓存所有节点 ID 以进行随机起点/终点选择
    for (const auto& nodePair : graph_.getNodes()) {
        allNodeIds_.push_back(nodePair.first);
    }
}

void TrafficSimulator::step() {
    changedEdges_.clear();
    stepCount_++;

    // 阶段 1：推进现有车辆
    advanceCars();

    // 阶段 2：生成新车辆
    spawnCars();
}

void TrafficSimulator::advanceCars() {
    std::vector<Car> surviving;
    surviving.reserve(activeCars_.size());

    for (auto& car : activeCars_) {
        car.remainingTime -= timeStep_;

        if (car.remainingTime <= 0.0) {
            // 车辆完成当前边的遍历
            Edge* currentEdge = graph_.getEdge(car.currentEdge);
            if (currentEdge) {
                currentEdge->decrementCarCount();
                changedEdges_.push_back(car.currentEdge);
            }

            // 移动到路线中的下一条边
            car.routeIndex++;
            if (car.routeIndex < car.route.size()) {
                Edge::Id nextEdgeId = car.route[car.routeIndex];
                Edge* nextEdge = graph_.getEdge(nextEdgeId);
                if (nextEdge) {
                    nextEdge->incrementCarCount();
                    car.currentEdge = nextEdgeId;
                    car.remainingTime = computeTravelTime(nextEdge);
                    changedEdges_.push_back(nextEdgeId);
                    surviving.push_back(car);
                }
                // 如果边无效，则移除车辆（不添加到 surviving）
            }
            // 如果路线完成，则移除车辆（不添加到 surviving）
        } else {
            // 车辆仍在当前边上行驶
            surviving.push_back(car);
        }
    }

    activeCars_ = std::move(surviving);
}

void TrafficSimulator::spawnCars() {
    if (allNodeIds_.size() < 2 || !pathfinder_) {
        return;
    }

    // 限制活动车辆数
    int toSpawn = spawnRate_;
    if (static_cast<int>(activeCars_.size()) + toSpawn > maxCars_) {
        toSpawn = maxCars_ - static_cast<int>(activeCars_.size());
    }
    if (toSpawn <= 0) {
        return;
    }

    std::uniform_int_distribution<size_t> nodeDist(0, allNodeIds_.size() - 1);

    for (int i = 0; i < toSpawn; ++i) {
        // 选择随机起点和终点
        Node::Id origin = allNodeIds_[nodeDist(rng_)];
        Node::Id destination = allNodeIds_[nodeDist(rng_)];

        // 如果是同一节点则跳过
        if (origin == destination) {
            continue;
        }

        // 计算路线
        PathResult result = pathfinder_->findPath(graph_, origin, destination);
        if (!result.found || result.pathEdges.empty()) {
            continue;
        }

        // 创建车辆并放置在第一条边上
        Car car;
        car.route = result.pathEdges;
        car.routeIndex = 0;
        car.currentEdge = car.route[0];

        Edge* firstEdge = graph_.getEdge(car.currentEdge);
        if (firstEdge) {
            firstEdge->incrementCarCount();
            car.remainingTime = computeTravelTime(firstEdge);
            changedEdges_.push_back(car.currentEdge);
            activeCars_.push_back(car);
        }
    }
}

double TrafficSimulator::computeTravelTime(const Edge* edge) const {
    if (!edge) return 1.0;
    double baseTime = TrafficModel::calculateTravelTime(
        edge->getLength(),
        edge->getCapacity(),
        static_cast<double>(edge->getCarCount())
    );
    return baseTime / roadClassSpeedFactor(edge->getRoadClass());
}

void TrafficSimulator::reset() {
    // 从边上移除所有车辆
    for (auto& car : activeCars_) {
        Edge* edge = graph_.getEdge(car.currentEdge);
        if (edge) {
            edge->decrementCarCount();
        }
    }
    activeCars_.clear();
    changedEdges_.clear();
    stepCount_ = 0;

    // 将所有边的车辆计数重置为零
    for (const auto& edgePair : graph_.getEdges()) {
        Edge* edge = graph_.getEdge(edgePair.first);
        if (edge) {
            edge->setCarCount(0);
        }
    }
}

} // namespace nav
