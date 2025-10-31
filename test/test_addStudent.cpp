#include <gtest/gtest.h>
#include "data/StudentDataHandler.h"
#include "DataModel/Student.h"
#include "data/PeopleDataHandler.h"
#include "DataModel/Person.h"
#include <QDate>

using namespace DataAccess;
using namespace DataModel;
Person p;
TEST(PeopleDataHandlerTest, AddPerson_valid)
{
    p.first_name = "Haider";
    p.second_name = "Bassem";
    p.third_name = "Nassif";
    p.fourth_name = "Jassim";
    p.date_of_birth = QDate(2004,9,20);
    p.gender = Gender::Male;

    auto result = PeopleDataHandler::addPerson(p);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(),1);
}

// TEST(StudentDataHandlerTest, AddStudent_Valid) {
//     Student s;

//     auto result = StudentDataHandler::addStudent(s);
//     ASSERT_TRUE(result.has_value());
//     EXPECT_EQ(result.value(), 1);
// }

// TEST(StudentDataHandlerTest, AddStudent_InvalidId) {
//     Student s{-1, "Ali"};
//     auto result = StudentDataHandler::addStudent(s);
//     ASSERT_FALSE(result.has_value());
// }
