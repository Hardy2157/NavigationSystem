#ifndef DISPLAYFILTER_H
#define DISPLAYFILTER_H

#include "core/graph/Edge.h"
#include <vector>

namespace nav {

// 缩放级别带的可见性规则
struct ZoomBand {
    double minZoom;          // 此带的最小缩放值
    double maxZoom;          // 此带的最大缩放值
    bool showLocalEdges;     // 是否显示支路
    bool showSecondaryEdges; // 是否显示次干道
    bool showArterialEdges;  // 是否显示主干道（始终 true）
    bool showNormalNodes;    // 是否显示普通节点
    int clusterLevel;        // 聚类层级 (-1=隐藏, 0=粗粒度, 1=细粒度)
};

// 缩放级别相关的可见性策略
class DisplayFilter {
public:
    DisplayFilter() {
        // 高缩放 (>= 1.0): 全部显示，隐藏聚类
        bands_.push_back({1.0, 1e9, true, true, true, true, -1});
        // 中缩放 (0.2 ~ 1.0): 隐藏支路和普通节点，细粒度聚类
        bands_.push_back({0.2, 1.0, false, true, true, false, 1});
        // 低缩放 (< 0.2): 仅主干道 + 粗粒度聚类
        bands_.push_back({0.0, 0.2, false, false, true, false, 0});
    }

    // 根据当前缩放级别获取可见性规则
    const ZoomBand& getBand(double zoom) const {
        for (const auto& band : bands_) {
            if (zoom >= band.minZoom && zoom < band.maxZoom) {
                return band;
            }
        }
        return bands_.front();  // 兜底：全部显示
    }

private:
    std::vector<ZoomBand> bands_;
};

} // namespace nav

#endif // DISPLAYFILTER_H
