cmake_minimum_required(VERSION 3.16)

project(Afq VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Sql)
find_package(Qt6 REQUIRED COMPONENTS Core)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(AfqApp
    main.cpp
    DataModel.h
    data/Logger.h data/Logger.cpp



    data/DatabaseManager/connectionPool.h data/DatabaseManager/connectionPool.cpp
    data/DatabaseManager/connectionWrapper.h data/DatabaseManager/connectionWrapper.cpp
    data/DatabaseManager/databaseManager.h data/DatabaseManager/databaseManager.cpp

    data/PeopleDataHandler.h data/PeopleDataHandler.cpp
)

qt_add_qml_module(AfqApp
    URI Afq
    VERSION 1.0
    QML_FILES
        Main.qml
        SOURCES data/LoggerWorker.h data/LoggerWorker.cpp
        SOURCES data/AcademicYearDataHandler.h data/AcademicYearDataHandler.cpp
        SOURCES
)

target_link_libraries(AfqApp
    PRIVATE
        Qt6::Quick
        Qt6::Sql
)
target_link_libraries(AfqApp PRIVATE Qt6::Core)
