cmake_minimum_required(VERSION 3.16)

project(Afq VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Quick Sql)


qt_standard_project_setup(REQUIRES 6.9.1)


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
    SOURCES
        data/LoggerWorker.h data/LoggerWorker.cpp
        data/AcademicYearDataHandler.h data/AcademicYearDataHandler.cpp
        data/GuardianDataHandler.h data/GuardianDataHandler.cpp
        data/StudentDataHandler.h data/StudentDataHandler.cpp
        DataModel/Student.h DataModel/Student.cpp
        DataModel/Person.h DataModel/Person.cpp
        DataModel/Guardian.h DataModel/Guardian.cpp
        DataModel/BaseEntity.h
        DataModel/StudentEnrollment.h DataModel/StudentEnrollment.cpp
        DataModel/Enrollment.h DataModel/Enrollment.cpp
        DataModel/StudentGuardian.h DataModel/StudentGuardian.cpp
        DataModel/AcademicYear.h DataModel/AcademicYear.cpp
        DataModel/Term.h DataModel/Term.cpp
        DataModel/EducationStage.h DataModel/EducationStage.cpp
        DataModel/Class.h DataModel/Class.cpp
        DataModel/Subject.h DataModel/Subject.cpp
        DataModel/Curriculum.h DataModel/Curriculum.cpp
        DataModel/CourseOffering.h DataModel/CourseOffering.cpp
        DataModel/Schedule.h DataModel/Schedule.cpp
        DataModel/Attendance.h DataModel/Attendance.cpp
        DataModel/Grade.h DataModel/Grade.cpp
        DataModel/DisciplinaryAction.h DataModel/DisciplinaryAction.cpp
        DataModel/Holiday.h DataModel/Holiday.cpp
        DataModel/Notification.h DataModel/Notification.cpp
        DataModel/Certificate.h DataModel/Certificate.cpp
        DataModel/ClassSummary.h DataModel/ClassSummary.cpp
        DataModel/StudentSummary.h DataModel/StudentSummary.cpp

)

target_link_libraries(AfqApp
    PRIVATE
        Qt6::Quick
        Qt6::Sql
)
target_link_libraries(AfqApp PRIVATE Qt6::Core Qt6::Quick Qt6::Sql)





# -------------------------------
# GoogleTest setup
# -------------------------------
#cmake_policy(SET CMP0135 NEW)
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable(googletest)

enable_testing()

# -------------------------------
# Test executable
# -------------------------------
add_executable(AfqTests
    test/test_addStudent.cpp
)

# Include source headers for tests
target_include_directories(AfqTests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(AfqTests
    PRIVATE
        GTest::gtest_main
        Qt6::Core
        Qt6::Sql
)

include(GoogleTest)
gtest_discover_tests(AfqTests)