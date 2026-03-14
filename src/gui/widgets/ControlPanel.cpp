#include "gui/widgets/ControlPanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>

namespace nav {

ControlPanel::ControlPanel(QWidget* parent)
    : QDockWidget("Control Panel", parent)
{
    setupUi();
}

void ControlPanel::setupUi() {
    QWidget* mainWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->setSpacing(10);

    // ========== Section 1: Spatial Query ==========
    QGroupBox* spatialGroup = new QGroupBox("Spatial Query (F1)", mainWidget);
    QVBoxLayout* spatialLayout = new QVBoxLayout(spatialGroup);

    QFormLayout* coordForm = new QFormLayout();

    xCoordSpinBox_ = new QDoubleSpinBox();
    xCoordSpinBox_->setRange(0.0, 10000.0);
    xCoordSpinBox_->setValue(5000.0);
    xCoordSpinBox_->setDecimals(2);
    xCoordSpinBox_->setSingleStep(100.0);
    coordForm->addRow("X Coordinate:", xCoordSpinBox_);

    yCoordSpinBox_ = new QDoubleSpinBox();
    yCoordSpinBox_->setRange(0.0, 10000.0);
    yCoordSpinBox_->setValue(5000.0);
    yCoordSpinBox_->setDecimals(2);
    yCoordSpinBox_->setSingleStep(100.0);
    coordForm->addRow("Y Coordinate:", yCoordSpinBox_);

    kNearestSpinBox_ = new QSpinBox();
    kNearestSpinBox_->setRange(1, 1000);
    kNearestSpinBox_->setValue(100);
    coordForm->addRow("K Nearest:", kNearestSpinBox_);

    spatialLayout->addLayout(coordForm);

    findNearestButton_ = new QPushButton("Find Nearest Nodes");
    spatialLayout->addWidget(findNearestButton_);

    spatialResultLabel_ = new QLabel("Result: -");
    spatialResultLabel_->setWordWrap(true);
    spatialResultLabel_->setStyleSheet("QLabel { background-color: #f0f0f0; padding: 5px; border-radius: 3px; }");
    spatialLayout->addWidget(spatialResultLabel_);

    mainLayout->addWidget(spatialGroup);

    // ========== Section 2: Path by ID ==========
    QGroupBox* pathGroup = new QGroupBox("Path by ID (F2/F4)", mainWidget);
    QVBoxLayout* pathLayout = new QVBoxLayout(pathGroup);

    QFormLayout* pathForm = new QFormLayout();

    startNodeSpinBox_ = new QSpinBox();
    startNodeSpinBox_->setRange(0, 99999);
    startNodeSpinBox_->setValue(0);
    pathForm->addRow("Start Node ID:", startNodeSpinBox_);

    endNodeSpinBox_ = new QSpinBox();
    endNodeSpinBox_->setRange(0, 99999);
    endNodeSpinBox_->setValue(9999);
    pathForm->addRow("End Node ID:", endNodeSpinBox_);

    // Routing criteria selection
    routingCriteriaCombo_ = new QComboBox();
    routingCriteriaCombo_->addItem("Shortest Distance (F2)", static_cast<int>(RoutingCriteria::ShortestDistance));
    routingCriteriaCombo_->addItem("Fastest Time (F4)", static_cast<int>(RoutingCriteria::FastestTime));
    routingCriteriaCombo_->setToolTip("F2: Distance-based (Dijkstra)\nF4: Traffic-aware (Dynamic)");
    pathForm->addRow("Routing Criteria:", routingCriteriaCombo_);

    pathLayout->addLayout(pathForm);

    computePathButton_ = new QPushButton("Compute Path");
    pathLayout->addWidget(computePathButton_);

    pathResultLabel_ = new QLabel("Result: -");
    pathResultLabel_->setWordWrap(true);
    pathResultLabel_->setStyleSheet("QLabel { background-color: #f0f0f0; padding: 5px; border-radius: 3px; }");
    pathLayout->addWidget(pathResultLabel_);

    mainLayout->addWidget(pathGroup);

    // ========== Clear Button ==========
    clearButton_ = new QPushButton("Clear All Highlights");
    clearButton_->setStyleSheet("QPushButton { background-color: #ffcccc; }");
    mainLayout->addWidget(clearButton_);

    // Add stretch to push everything to top
    mainLayout->addStretch();

    setWidget(mainWidget);

    // Set minimum width
    setMinimumWidth(250);

    // Connect signals
    connect(findNearestButton_, &QPushButton::clicked, this, &ControlPanel::onFindNearestClicked);
    connect(computePathButton_, &QPushButton::clicked, this, &ControlPanel::onComputePathClicked);
    connect(clearButton_, &QPushButton::clicked, this, &ControlPanel::onClearClicked);
}

void ControlPanel::setNodeIdRange(int minId, int maxId) {
    startNodeSpinBox_->setRange(minId, maxId);
    endNodeSpinBox_->setRange(minId, maxId);
    endNodeSpinBox_->setValue(maxId);
}

void ControlPanel::setCoordinateRange(double minX, double maxX, double minY, double maxY) {
    xCoordSpinBox_->setRange(minX, maxX);
    yCoordSpinBox_->setRange(minY, maxY);
    xCoordSpinBox_->setValue((minX + maxX) / 2.0);
    yCoordSpinBox_->setValue((minY + maxY) / 2.0);
}

void ControlPanel::onFindNearestClicked() {
    double x = xCoordSpinBox_->value();
    double y = yCoordSpinBox_->value();
    int k = kNearestSpinBox_->value();

    spatialResultLabel_->setText("Searching...");
    emit findNearestRequested(x, y, k);
}

void ControlPanel::onComputePathClicked() {
    uint32_t startId = static_cast<uint32_t>(startNodeSpinBox_->value());
    uint32_t endId = static_cast<uint32_t>(endNodeSpinBox_->value());
    RoutingCriteria criteria = getRoutingCriteria();

    pathResultLabel_->setText("Computing...");
    emit computePathRequested(startId, endId, criteria);
}

RoutingCriteria ControlPanel::getRoutingCriteria() const {
    int index = routingCriteriaCombo_->currentData().toInt();
    return static_cast<RoutingCriteria>(index);
}

void ControlPanel::onClearClicked() {
    spatialResultLabel_->setText("Result: -");
    pathResultLabel_->setText("Result: -");
    emit clearHighlightsRequested();
}

void ControlPanel::showSpatialQueryResult(int nodeCount, int edgeCount) {
    spatialResultLabel_->setText(QString("Found %1 nodes\nConnected edges: %2")
                                     .arg(nodeCount)
                                     .arg(edgeCount));
}

void ControlPanel::showPathResult(int nodeCount, double totalCost, bool found) {
    if (found) {
        pathResultLabel_->setText(QString("Path found!\nNodes: %1\nTotal cost: %2")
                                      .arg(nodeCount)
                                      .arg(totalCost, 0, 'f', 2));
    } else {
        pathResultLabel_->setText("No path found!");
    }
}

} // namespace nav
