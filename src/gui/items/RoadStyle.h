#ifndef ROADSTYLE_H
#define ROADSTYLE_H

#include <QColor>
#include "core/graph/Edge.h"

namespace nav {

// 道路等级的视觉样式参数
struct RoadStyle {
    QColor color;          // 默认颜色（无交通热力图时）
    double width;          // 线宽
    double zValue;         // 绘制层级

    // 获取指定道路等级的预定义样式
    static RoadStyle forRoadClass(RoadClass rc) {
        switch (rc) {
            case RoadClass::Arterial:
                // 深蓝灰色，宽3.0，z=2
                return {QColor(55, 71, 79), 3.0, 2.0};
            case RoadClass::Secondary:
                // 中灰色，宽2.0，z=1
                return {QColor(120, 144, 156), 2.0, 1.0};
            case RoadClass::Local:
            default:
                // 浅灰绿色，宽1.0，z=0
                return {QColor(176, 190, 176), 1.0, 0.0};
        }
    }

    // 拥堵状态下根据道路等级调整线宽
    static double congestionWidth(RoadClass rc, int status) {
        double base = forRoadClass(rc).width;
        switch (status) {
            case 1: return base + 0.5;  // 黄色：稍粗
            case 2: return base + 1.0;  // 红色：更粗
            default: return base;
        }
    }
};

} // namespace nav

#endif // ROADSTYLE_H
