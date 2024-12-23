cmake_minimum_required(VERSION 3.28)
project(newPJGraph)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt5 COMPONENTS
        Core
        Gui
        Sql
        Widgets
        REQUIRED)

add_executable(newPJGraph main.cpp
        DatabaseManager.cpp
        DatabaseManager.h
        WayHead.cpp
        WayHead.h
        Car.cpp
        Car.h
        AdjacencyList.cpp
        AdjacencyList.h
        MainWidget.cpp
        MainWidget.h
        CustomScene.cpp
        CustomScene.h
        CustomGraphicsView.cpp
        CustomGraphicsView.h
        Node.h
        Way.h
        WayType.h)
target_link_libraries(newPJGraph
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::Sql
)

