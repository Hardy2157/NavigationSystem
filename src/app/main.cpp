#include <QApplication>
#include "gui/MainWindow.h"
#include <iostream>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    std::cout << "=== Navigation System - Phase 6: Qt GUI ===" << std::endl;

    // Create and show main window
    nav::MainWindow mainWindow;
    mainWindow.show();

    // Generate initial map
    std::cout << "Generating initial map..." << std::endl;
    mainWindow.generateNewMap(10000, 10000.0, 10000.0);

    std::cout << "GUI ready. Use mouse wheel to zoom, drag to pan." << std::endl;
    std::cout << "Press Ctrl+G to generate a new map." << std::endl;

    return app.exec();
}
