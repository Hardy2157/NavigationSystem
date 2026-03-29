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
    // 将图保存到文件。成功时返回 true。
    static bool save(const std::string& filepath, const Graph& graph,
                     double mapWidth, double mapHeight);

    // 从文件加载图。成功时返回 true，填充 outData。
    static bool load(const std::string& filepath, MapFileData& outData);

    // 获取最后的错误消息（用于 UI 显示）
    static const std::string& getLastError();

private:
    static std::string lastError_;
};

} // namespace nav

#endif // GRAPHIO_H
