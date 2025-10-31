cmake_minimum_required(VERSION 3.16)

project(AFQ VERSION 0.1 LANGUAGES CXX)

# -------------------------------
# Compiler setup (Clang preferred)
# -------------------------------

find_program(CLANG_C clang)
find_program(CLANG_CXX clang++)
if(CLANG_C AND CLANG_CXX)
    message(STATUS "Using Clang compiler")
    set(CMAKE_C_COMPILER ${CLANG_C})
    set(CMAKE_CXX_COMPILER ${CLANG_CXX})
else()
    message(STATUS "Clang not found, using default compiler")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# -------------------------------
# Qt setup
# -------------------------------
find_package(Qt6 REQUIRED COMPONENTS Core Quick Sql)
qt_standard_project_setup(REQUIRES 6.9.1)


# -------------------------------
# Main application
# -------------------------------
qt_add_executable(AFQ
    main.cpp
    data/Logger.cpp
    data/DatabaseManager/connectionPool.cpp
    data/DatabaseManager/connectionWrapper.cpp
    data/DatabaseManager/databaseManager.cpp
    data/PeopleDataHandler.cpp

    data/Logger.h
    data/DatabaseManager/connectionPool.h
    data/DatabaseManager/connectionWrapper.h
    data/DatabaseManager/databaseManager.h
    data/PeopleDataHandler.h
)

qt_add_qml_module(AFQ
    URI Afq
    VERSION 1.0
    QML_FILES Main.qml
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

        data/LoggerWorker.h
        data/AcademicYearDataHandler.h
        data/GuardianDataHandler.h
        data/StudentDataHandler.h

        DataModel/Student.h
        DataModel/Person.h
        DataModel/Guardian.h
        DataModel/StudentEnrollment.h
        DataModel/Enrollment.h
        DataModel/StudentGuardian.h
        DataModel/AcademicYear.h
        DataModel/Term.h
        DataModel/EducationStage.h
        DataModel/Class.h
        DataModel/Subject.h
        DataModel/Curriculum.h
        DataModel/CourseOffering.h
        DataModel/Schedule.h
        DataModel/Attendance.h
        DataModel/Grade.h
        DataModel/DisciplinaryAction.h
        DataModel/Holiday.h
        DataModel/Notification.h
        DataModel/Certificate.h
        DataModel/ClassSummary.h
        DataModel/StudentSummary.h
)

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

add_executable(AfqTests
    test/test_addStudent.cpp
    test/test_main.cpp

    data/PeopleDataHandler.cpp
    data/Logger.cpp
    data/LoggerWorker.cpp
    data/DatabaseManager/connectionPool.cpp
    data/DatabaseManager/connectionWrapper.cpp
    data/DatabaseManager/databaseManager.cpp
    
    DataModel/Person.cpp
)

target_include_directories(AfqTests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/data
    ${CMAKE_CURRENT_SOURCE_DIR}/DataModel
)

target_link_libraries(AfqTests
    PRIVATE GTest::gtest_main Qt6::Core Qt6::Sql
)

include(GoogleTest)
gtest_discover_tests(AfqTests)