#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <QDockWidget>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include "core/graph/Node.h"
#include <vector>

namespace nav {

// 路由策略枚举
enum class RoutingCriteria {
    ShortestDistance = 0,  // F2: 使用 DijkstraPathFinder（基于距离）
    FastestTime = 1        // F4: 使用 DynamicPathFinder（交通感知）
};

class ControlPanel : public QDockWidget {
    Q_OBJECT

public:
    explicit ControlPanel(QWidget* parent = nullptr);

    // 设置节点 ID 的有效范围
    void setNodeIdRange(int minId, int maxId);

    // 设置坐标范围
    void setCoordinateRange(double minX, double maxX, double minY, double maxY);

    // 获取当前路由策略
    RoutingCriteria getRoutingCriteria() const;

signals:
    // 空间查询信号
    void findNearestRequested(double x, double y, int k);

    // 路径查找信号（现在包括路由策略）
    void computePathRequested(uint32_t startId, uint32_t endId, RoutingCriteria criteria);

    // 交通视图信号
    void showTrafficNearRequested(double x, double y, double radius);

    // 清除信号
    void clearHighlightsRequested();

public slots:
    // 更新结果显示
    void showSpatialQueryResult(int nodeCount, int edgeCount);
    void showPathResult(int nodeCount, double totalCost, bool found);
    void showTrafficResult(int edgeCount);

private slots:
    void onFindNearestClicked();
    void onComputePathClicked();
    void onShowTrafficClicked();
    void onClearClicked();

private:
    void setupUi();

    // 空间查询部分
    QDoubleSpinBox* xCoordSpinBox_;
    QDoubleSpinBox* yCoordSpinBox_;
    QSpinBox* kNearestSpinBox_;
    QPushButton* findNearestButton_;
    QLabel* spatialResultLabel_;

    // 按 ID 查找路径部分
    QSpinBox* startNodeSpinBox_;
    QSpinBox* endNodeSpinBox_;
    QComboBox* routingCriteriaCombo_;
    QPushButton* computePathButton_;
    QLabel* pathResultLabel_;

    // 交通视图部分
    QDoubleSpinBox* trafficXSpinBox_;
    QDoubleSpinBox* trafficYSpinBox_;
    QDoubleSpinBox* trafficRadiusSpinBox_;
    QPushButton* showTrafficButton_;
    QLabel* trafficResultLabel_;

    // 清除按钮
    QPushButton* clearButton_;
};

} // namespace nav

#endif // CONTROLPANEL_H
