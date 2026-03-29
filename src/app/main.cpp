#include <QApplication>
#include "gui/MainWindow.h"
#include <iostream>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    

    // 创建并显示主窗口
    nav::MainWindow mainWindow;
    mainWindow.show();

    // 生成初始地图
    std::cout << "Generating initial map..." << std::endl;
    mainWindow.generateNewMap(10000, 10000.0, 10000.0);

    std::cout << "GUI ready. Use mouse wheel to zoom, drag to pan." << std::endl;
    std::cout << "Press Ctrl+G to generate a new map." << std::endl;

    return app.exec();
}
