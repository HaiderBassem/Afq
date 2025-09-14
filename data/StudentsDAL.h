#ifndef STUDENTSDAL_H
#define STUDENTSDAL_H
// #include<optional>
// #include<string>
// #include<pqxx/pqxx>
// #include<QDate>

// #include"personDAL.h"

// namespace DataAccess{



// class studentsDAL
// {
// public:

//     static std::optional<int> enrollNewStudent(
//         const std::string& firstName,
//         const std::string& secondName,
//         const std::string& thirdName,
//         const std::string& fourthName,
//         const char& gender,
//         const QDate& dateOfBirth,
//         const std::string& address,
//         const std::string& phoneOne,
//         const std::string& phoneTwo,
//         const std::string& email,
//         const std::string& imagePath
//         );


//     static std::optional<int> transferStudentIn(
//         const std::string& firstName,
//         const std::string& secondName,
//         const std::string& thirdName,
//         const std::string& fourthName,
//         char gender,
//         const QDate& dateOfBirth,
//         const std::string& address,
//         const std::string& phoneOne,
//         const std::string& phoneTwo,
//         const std::string& email,
//         const std::string& imagePath,
//         const std::string& fromSchool,
//         const std::string& stage,
//         const int& yearsOfFail
//         );


//     static bool transferStudentOut(const int& id, const std::string& toSchool);

//     static bool getStudentById(
//         const int& id,
//         std::string& firstName,
//         std::string& secondName,
//         std::string& thirdName,
//         std::string& fourthName,
//         char& gender,
//         QDate& dateOfBirth,
//         std::string& address,
//         std::string& phoneOne,
//         std::string& phoneTwo,
//         std::string& email,
//         std::string& imagePath
//         );


//     static bool updateStudent(
//         const int& id,
//         const std::string& firstName,
//         const std::string& secondName,
//         const std::string& thirdName,
//         const std::string& fourthName,
//         const char& gender,
//         const QDate& dateOfBirth,
//         const std::string& address,
//         const std::string& phoneOne,
//         const std::string& phoneTwo,
//         const std::string& email,
//         const std::string& imagePath
//         );

//     static bool deleteStudent(const int& id);



//     // Get all Students from DB and store them in vector of maps
//     static std::vector<std::map<std::string, std::string>> getAllStudents();

//     static bool isStudentExist(const int& id);

//         // prepere all stetments of people calss
//     static void initStatements(pqxx::connection* conn);

// };
// }
#endif // STUDENTSDAL_H
