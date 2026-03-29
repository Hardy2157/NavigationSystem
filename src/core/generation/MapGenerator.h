#ifndef MAPGENERATOR_H
#define MAPGENERATOR_H

#include "core/graph/Graph.h"

namespace nav {

// 地图生成算法的抽象基类
class MapGenerator {
public:
    virtual ~MapGenerator() = default;

    // 生成具有指定节点数的地图
    // @param graph 要填充节点和边的图
    // @param numNodes 要生成的目标节点数
    // @param width 地图区域的宽度
    // @param height 地图区域的高度
    virtual void generate(Graph& graph, int numNodes, double width, double height) = 0;
};

} // namespace nav

#endif // MAPGENERATOR_H
