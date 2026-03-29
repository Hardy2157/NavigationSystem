#include "core/traffic/TrafficModel.h"
#include <cmath>

namespace nav {

double TrafficModel::calculateTravelTime(double length, double capacity, double currentCount) {
    // 避免除以零
    if (capacity <= 0.0) {
        return length * C;  // 如果容量无效则默认为畅通
    }

    double ratio = currentCount / capacity;  // x = n/v

    if (ratio <= K) {
        // 畅通：T = C * L * 1.0
        return C * length * 1.0;
    } else {
        // 拥堵：T = C * L * (1.0 + exp(x))
        return C * length * (1.0 + std::exp(ratio));
    }
}

double TrafficModel::getCongestionRatio(double capacity, double currentCount) {
    if (capacity <= 0.0) {
        return 0.0;
    }
    return currentCount / capacity;
}

int TrafficModel::getCongestionStatus(double capacity, double currentCount) {
    double ratio = getCongestionRatio(capacity, currentCount);

    if (ratio < VISUAL_YELLOW_RATIO) {
        return 0;  // 绿色 - 低负载
    } else if (ratio < VISUAL_RED_RATIO) {
        return 1;  // 黄色 - 中等拥堵
    } else {
        return 2;  // 红色 - 严重拥堵
    }
}

} // namespace nav
