// #include "studentsDAL.h"


// void DataAccess::studentsDAL::initStatements(pqxx::connection *conn)
// {
//     conn->prepare("insert_enrollment", "INSERT INTO enrollment "
//                                 "(person_id, school_id, role, start_date, status)"
//                                 "VALUES ($1 ,$2 , 'Student', CURRENT_DATE, 'Active' ) "
//                                 " RETURNING enrollment_id; ");

//     conn->prepare("insert_student_enrollment", "INSERT INTO student_enrollment"
//                                         " (enrollment_id, calss_id, year_id, start_date, status) "
//                                                " VALUES($1, $2, $3, CURRENT_DATE, 'Active' ) ");

// }
