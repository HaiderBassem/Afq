
#pragma once

#include "DataModel/Guardian.h"

#include<QString>
#include<QDate>
#include<optional>
#include"DataModel/Guardian.h"
#include"DataModel/StudentGuardian.h"

namespace DataAccess {


class GuardianDataHandler
{
public:

    //CRUD
    static bool addGuardian(const DataModel::Guardian& guardian);
    static bool updateGuardin(int duradinId, const DataModel::Guardian& guardian);
    static bool deleteGuardian(int guradianId);
    static DataModel::Guardian getGuardianById(int guardianId);
    static QVector<DataModel::Guardian> getAllGuardiansById();
    // relations
    static bool addStudentGuardianRelationship(int studentPersonId, int guradianId, const QString& relationship, bool isPrimary = false);
    static bool removeStudentGuardianRelationship(int studentGuardianId);
    static bool updateStudentGuardianRelationship(int studentGuradianId, const QString& relationship, bool isPrimary);
    static bool setPrimaryGuardian(int studentPersonId, int guardianId);
    static QVector<DataModel::StudentGuardian> getStudentGuardians(int studentPersonId);
    static DataModel::Guardian getPrimaryGuardianByStudent(int studentPersonId);

    //searching
    static QVector<DataModel::Guardian> searchGuardianByPhoneNumber(const QString& phoneNumber);
    static QVector<DataModel::Guardian> searchGuardianByName(const QString& fname, const QString& sname = "", const QString& tname = "");
    static QVector<DataModel::Guardian> searchGuardianByOccupation(const QString& occupation);
    static QVector<DataModel::Guardian> searchGuardiansByEducationLevel(const QString& educationLevel);
    static QVector<DataModel::Guardian> searchGuardiansByStudentName(const QString& f_studentName, const QString& s_studentName = "", const QString& t_studentName = "", const QString& ft_studentName ="");

    // statistics
    static std::optional<int> getGuardiansCount();
   // static QVector<DataModel::Guardian> getGuardiansWithMultipleStudents();


    // checks
    static bool isGuardianExists(int guardianId);
    static bool isStudentGuardianRelationshipExists(int studentPersonId, int guardianId);
    static bool hasStudentAnyGuardian(int studentPersonId);
    static DataModel::Guardian getPrimaryGuardianInfo(int studentPersonId);
    static bool hasPrimaryGuardian(int studentPersonId);




    //updating

    static bool updateGuardinOccupation(int guardianId, const QString& occupation);
    static bool updateGuardinPhoneNumber(int guardianId, const QString& phoneNumber);
    static bool updateGuardinEducationLevel(int guardianId, const QString& educationLevel);


private:
    // to prevent create instance
    // GuardianDataHandler() = delete;
    // ~GuardianDataHandler() = delete;
};
}
