#ifndef TRAFFICMODEL_H
#define TRAFFICMODEL_H

namespace nav {

// 交通计算的无状态工具类
// 使用公式: T = C * L * f(x) 其中 x = n/v (车辆数/容量)
// f(x) = 1.0 如果 x <= K, 否则 1.0 + exp(x)
class TrafficModel {
public:
    // 常量
    static constexpr double C = 1.0;  // 时间常数
    static constexpr double K = 1.0;  // 拥堵阈值
    static constexpr double VISUAL_YELLOW_RATIO = 0.25;  // 在热力图中更早显示中等负载
    static constexpr double VISUAL_RED_RATIO = 0.5;      // 可视化的重度负载

    // 计算边的行驶时间
    // length: 边的物理长度 (L)
    // capacity: 最大车辆容量 (v)
    // currentCount: 边上当前的车辆数 (n)
    // 返回: 行驶时间 T = C * L * f(x) 其中 x = n/v
    static double calculateTravelTime(double length, double capacity, double currentCount);

    // 获取拥堵比率 (n/v)
    static double getCongestionRatio(double capacity, double currentCount);

    // 获取可视化的拥堵状态
    // 返回: 0 = 绿色（畅通）, 1 = 黄色（中等）, 2 = 红色（拥堵）
    static int getCongestionStatus(double capacity, double currentCount);
};

} // namespace nav

#endif // TRAFFICMODEL_H
