#include "gui/widgets/GenerateMapDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>

namespace nav {

GenerateMapDialog::GenerateMapDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("生成新地图");
    setupUi();
}

void GenerateMapDialog::setupUi() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Settings group
    QGroupBox* settingsGroup = new QGroupBox("地图设置", this);
    QFormLayout* formLayout = new QFormLayout(settingsGroup);

    // Node count (minimum 10,000)
    nodeCountSpinBox_ = new QSpinBox();
    nodeCountSpinBox_->setRange(10000, 1000000);
    nodeCountSpinBox_->setValue(10000);
    nodeCountSpinBox_->setSingleStep(1000);
    nodeCountSpinBox_->setToolTip("节点数量（最少 10,000）");
    formLayout->addRow("节点数量 (N >= 10000):", nodeCountSpinBox_);

    // Map width
    widthSpinBox_ = new QDoubleSpinBox();
    widthSpinBox_->setRange(1000.0, 100000.0);
    widthSpinBox_->setValue(10000.0);
    widthSpinBox_->setSingleStep(1000.0);
    widthSpinBox_->setDecimals(0);
    formLayout->addRow("地图宽度:", widthSpinBox_);

    // Map height
    heightSpinBox_ = new QDoubleSpinBox();
    heightSpinBox_->setRange(1000.0, 100000.0);
    heightSpinBox_->setValue(10000.0);
    heightSpinBox_->setSingleStep(1000.0);
    heightSpinBox_->setDecimals(0);
    formLayout->addRow("地图高度:", heightSpinBox_);

    mainLayout->addWidget(settingsGroup);

    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    generateButton_ = new QPushButton("生成");
    cancelButton_ = new QPushButton("取消");
    buttonLayout->addStretch();
    buttonLayout->addWidget(generateButton_);
    buttonLayout->addWidget(cancelButton_);
    mainLayout->addLayout(buttonLayout);

    // Connect buttons
    connect(generateButton_, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton_, &QPushButton::clicked, this, &QDialog::reject);

    setMinimumWidth(350);
}

int GenerateMapDialog::getNodeCount() const {
    return nodeCountSpinBox_->value();
}

double GenerateMapDialog::getMapWidth() const {
    return widthSpinBox_->value();
}

double GenerateMapDialog::getMapHeight() const {
    return heightSpinBox_->value();
}

} // namespace nav
