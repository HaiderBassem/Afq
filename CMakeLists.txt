cmake_minimum_required(VERSION 3.16)

project(AFQ VERSION 0.1 LANGUAGES CXX)

# Use Clang if available
find_program(CLANG_C clang)
find_program(CLANG_CXX clang++)
if(CLANG_C AND CLANG_CXX)
    message(STATUS "Using Clang compiler")
    set(CMAKE_C_COMPILER ${CLANG_C})
    set(CMAKE_CXX_COMPILER ${CLANG_CXX})
else()
    message(STATUS "Clang not found, using default compiler")
endif()

# C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Quick Sql)
qt_standard_project_setup(REQUIRES 6.9.1)

# Main executable
qt_add_executable(AFQ
    main.cpp
    data/Logger.cpp
    data/DatabaseManager/connectionPool.cpp
    data/DatabaseManager/connectionWrapper.cpp
    data/DatabaseManager/databaseManager.cpp
    data/PeopleDataHandler.cpp
)

# QML module
qt_add_qml_module(AFQ
    URI Afq
    VERSION 1.0
    QML_FILES
        Main.qml
    SOURCES
        data/LoggerWorker.cpp
        data/AcademicYearDataHandler.cpp
        data/GuardianDataHandler.cpp
        data/StudentDataHandler.cpp
        DataModel/Student.cpp
        DataModel/Person.cpp
        DataModel/Guardian.cpp
        DataModel/StudentEnrollment.cpp
        DataModel/Enrollment.cpp
        DataModel/StudentGuardian.cpp
        DataModel/AcademicYear.cpp
        DataModel/Term.cpp
        DataModel/EducationStage.cpp
        DataModel/Class.cpp
        DataModel/Subject.cpp
        DataModel/Curriculum.cpp
        DataModel/CourseOffering.cpp
        DataModel/Schedule.cpp
        DataModel/Attendance.cpp
        DataModel/Grade.cpp
        DataModel/DisciplinaryAction.cpp
        DataModel/Holiday.cpp
        DataModel/Notification.cpp
        DataModel/Certificate.cpp
        DataModel/ClassSummary.cpp
        DataModel/StudentSummary.cpp

)

# Link libraries
target_link_libraries(AFQ
    PRIVATE Qt6::Core Qt6::Quick Qt6::Sql
)

# -------------------------------
# GoogleTest setup
# -------------------------------
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(googletest)
enable_testing()

# Test executable
add_executable(AfqTests test/test_addStudent.cpp)
target_include_directories(AfqTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(AfqTests PRIVATE GTest::gtest_main Qt6::Core Qt6::Sql)
include(GoogleTest)
gtest_discover_tests(AfqTests)
