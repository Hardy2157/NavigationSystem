#include "core/io/GraphIO.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <locale>

namespace nav {

std::string GraphIO::lastError_;

bool GraphIO::save(const std::string& filepath, const Graph& graph,
                   double mapWidth, double mapHeight) {
    std::ofstream out(filepath);
    if (!out.is_open()) {
        lastError_ = "Cannot open file for writing: " + filepath;
        return false;
    }

    out.imbue(std::locale::classic());

    out << std::fixed << std::setprecision(10);

    // 头部
    out << "NAVMAP 2.0\n";
    out << "WIDTH " << mapWidth << "\n";
    out << "HEIGHT " << mapHeight << "\n";
    out << "NODES " << graph.getNodeCount() << "\n";
    out << "EDGES " << graph.getEdgeCount() << "\n";

    // 节点
    for (const auto& pair : graph.getNodes()) {
        const Node& node = pair.second;
        out << "NODE " << node.getId()
            << " " << node.getPosition().x
            << " " << node.getPosition().y << "\n";
    }

    // 边
    for (const auto& pair : graph.getEdges()) {
        const Edge& edge = pair.second;
        out << "EDGE " << edge.getId()
            << " " << edge.getSource()
            << " " << edge.getTarget()
            << " " << edge.getLength()
            << " " << edge.getCapacity()
            << " " << static_cast<int>(edge.getRoadClass()) << "\n";
    }

    out << "END\n";

    if (!out.good()) {
        lastError_ = "Write error while saving to: " + filepath;
        return false;
    }

    return true;
}

bool GraphIO::load(const std::string& filepath, MapFileData& outData) {
    std::ifstream in(filepath);
    if (!in.is_open()) {
        lastError_ = "Cannot open file for reading: " + filepath;
        return false;
    }

    in.imbue(std::locale::classic());

    outData.graph.clear();
    outData.width = 0.0;
    outData.height = 0.0;

    size_t expectedNodes = 0;
    size_t expectedEdges = 0;
    std::string line;
    int lineNum = 0;
    const std::locale classic = std::locale::classic();

    // 读取头部
    if (!std::getline(in, line)) {
        lastError_ = "Empty file";
        return false;
    }
    lineNum++;
    if (line.substr(0, 6) != "NAVMAP") {
        lastError_ = "Invalid file format: missing NAVMAP header";
        return false;
    }
    // 检测版本号（1.0 或 2.0）
    bool isVersion2 = (line.find("2.0") != std::string::npos);

    // 读取元数据行
    while (std::getline(in, line)) {
        lineNum++;
        std::istringstream iss(line);
        iss.imbue(classic);
        std::string keyword;
        iss >> keyword;

        if (keyword == "WIDTH") {
            iss >> outData.width;
        } else if (keyword == "HEIGHT") {
            iss >> outData.height;
        } else if (keyword == "NODES") {
            iss >> expectedNodes;
        } else if (keyword == "EDGES") {
            iss >> expectedEdges;
        } else if (keyword == "NODE" || keyword == "EDGE" || keyword == "END") {
            // 第一个数据行 — 在下面处理
            break;
        }
    }

    if (expectedNodes > 0 || expectedEdges > 0) {
        outData.graph.reserve(expectedNodes, expectedEdges);
    }

    // 处理数据行（第一行已在 'line' 中）
    do {
        std::istringstream iss(line);
        iss.imbue(classic);
        std::string keyword;
        iss >> keyword;

        if (keyword == "NODE") {
            Node::Id id;
            double x, y;
            if (!(iss >> id >> x >> y)) {
                lastError_ = "Parse error at line " + std::to_string(lineNum) + ": invalid NODE";
                return false;
            }
            outData.graph.addNodeWithId(id, Point2D(x, y));
        } else if (keyword == "EDGE") {
            Edge::Id id;
            Node::Id source, target;
            double length, capacity;
            if (!(iss >> id >> source >> target >> length >> capacity)) {
                lastError_ = "Parse error at line " + std::to_string(lineNum) + ": invalid EDGE";
                return false;
            }
            // v2 格式包含道路等级字段
            RoadClass roadClass = RoadClass::Local;
            if (isVersion2) {
                int rc = 2;
                if (iss >> rc) {
                    roadClass = static_cast<RoadClass>(rc);
                }
            }
            if (outData.graph.addEdgeWithId(id, source, target, length, capacity, roadClass) == Edge::INVALID_ID) {
                lastError_ = "Parse error at line " + std::to_string(lineNum) + ": edge references missing node";
                return false;
            }
        } else if (keyword == "END") {
            break;
        }

        lineNum++;
    } while (std::getline(in, line));

    if (outData.graph.getNodeCount() == 0) {
        lastError_ = "File contains no nodes";
        return false;
    }

    return true;
}

const std::string& GraphIO::getLastError() {
    return lastError_;
}

} // namespace nav
