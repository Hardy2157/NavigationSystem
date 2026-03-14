cmake_minimum_required(VERSION 3.16)
project(NavigationSystem VERSION 1.0.0 LANGUAGES CXX)

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Qt MOC, UIC, and RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find Qt5 Widgets
find_package(Qt5 5.14 REQUIRED COMPONENTS Widgets)

# Add source directory
add_subdirectory(src)
