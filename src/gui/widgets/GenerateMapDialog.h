#ifndef GENERATEMAPDIALOG_H
#define GENERATEMAPDIALOG_H

#include <QDialog>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>

namespace nav {

class GenerateMapDialog : public QDialog {
    Q_OBJECT

public:
    explicit GenerateMapDialog(QWidget* parent = nullptr);

    int getNodeCount() const;
    double getMapWidth() const;
    double getMapHeight() const;

private:
    void setupUi();

    QSpinBox* nodeCountSpinBox_;
    QDoubleSpinBox* widthSpinBox_;
    QDoubleSpinBox* heightSpinBox_;
    QPushButton* generateButton_;
    QPushButton* cancelButton_;
};

} // namespace nav

#endif // GENERATEMAPDIALOG_H
