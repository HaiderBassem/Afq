#include "AcademicYearDAL.h"
#include "DatabaseManager/databaseManager.h"
#include<QSqlDatabase>
#include<QSqlError>
#include<QSqlQuery>
std::optional<int> DataAccess::AcademicYearDAL::createAcademicYear(const QString &name, const QDate &start_date, const QDate &end_date, bool isActive)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    QSqlQuery query(db);
    query.prepare(R"(
    INSERT INTO academic_year(name, start_date, end_date, is_active)
    VALUES(:name, :start, :end, :active)   RETURNING year_id)");

    query.bindValue(":name",name);
    query.bindValue(":start", start_date.toString("yyyy-MM-dd"));
    query.bindValue(":end",end_date.toString("yyyy-MM-dd"));
    query.bindValue(":active", isActive);

    if(query.exec() && query.next())
        return query.value(0).toInt();
    else
    {

        qWarning() <<"CREATE ACADEMIC YEAR ERROR:"<<query.lastError().text();
    return std::nullopt;
    }
}

std::optional<DataModel::AcademicYear> DataAccess::AcademicYearDAL::getAcademicYearById(int id)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare("SELECT * FROM academic_year WHERE year_id = :id");
    query.bindValue(":id", id);

    while(!query.exec())
    {
        qWarning()<<"GET ACADEMIC YEAR BY ID ERROR:"  << query.lastError().text();
        return std::nullopt;
    }
    if(!query.next()) return std::nullopt;

    DataModel::AcademicYear AcademicYear;
    AcademicYear.year_id = query.value("year_id").toInt();
    AcademicYear.name = query.value("name").toString();
    AcademicYear.start_date = query.value("start_date").toDate();
    AcademicYear.end_date = query.value("end_date").toDate();
    AcademicYear.is_active = query.value("is_active").toBool();

    return AcademicYear;
}

bool DataAccess::AcademicYearDAL::uodateAcademicYear(int id, const QString &name, const QDate &start_date, const QDate &end_date)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare("UPDATE academic_year SET name = :name, start_date = :start, end_date = :end WHERE year_id = :id ");

    query.bindValue(":id",id);
    query.bindValue(":name",name);
    query.bindValue(":start", start_date);
    query.bindValue(":end",end_date);

    if(!query.exec()) {qWarning()<< " UPDATE ACAEMIC YEAR ERROR:"<<query.lastError().text(); return false;}

    return query.numRowsAffected() > 0;
}

bool DataAccess::AcademicYearDAL::deleteAcademicYear(int id)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);


    query.prepare("DELETE FROM academic_year WHERE year_id = :id");

    query.bindValue(":id",id);

     if(!query.exec()) {qWarning()<< " DELETE ACAEMIC YEAR ERROR:"<<query.lastError().text(); return false;}

     return query.numRowsAffected() > 0;
}

QVector<DataModel::AcademicYear> DataAccess::AcademicYearDAL::getAllAcademicYears()
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);


    query.prepare("SELETC * FROM academic_year");

    QVector<DataModel::AcademicYear> AcademicYears;
    if(!query.exec()) {qWarning()<<"GET ALL ACADEMIC YEARS ERROR:" << query.lastError().text(); return AcademicYears;}

    while(query.next())
    {
        DataModel::AcademicYear AcademicYear;
        AcademicYear.year_id = query.value("year_id").toInt();
        AcademicYear.name = query.value("name").toString();
        AcademicYear.start_date = query.value("start_date").toDate();
        AcademicYear.end_date = query.value("end_date").toDate();
        AcademicYear.is_active = query.value("is_active").toBool();
        AcademicYears.append(AcademicYear);
    }
    return AcademicYears;

}

std::optional<DataModel::AcademicYear> DataAccess::AcademicYearDAL::getCurrentAcademicYear()
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare("SELECT * FROM academic_year WHERE start_date <= CURRENT_DATE AND end_date >= CURRENT_DATE LIMIT 1 ");

    if (!query.exec())
    {
        qWarning() << "GET CURRENT ACADEMIC YEAR ERROR:" << query.lastError().text();
        return std::nullopt;
    }

    if(query.next())
    {
        DataModel::AcademicYear AcademicYear;
        AcademicYear.year_id = query.value("year_id").toInt();
        AcademicYear.name = query.value("name").toString();
        AcademicYear.start_date = query.value("start_date").toDate();
        AcademicYear.end_date = query.value("end_date").toDate();
        AcademicYear.is_active = query.value("is_active").toBool();
        return AcademicYear;
    }

    return std::nullopt;
}


std::optional<DataModel::AcademicYear> DataAccess::AcademicYearDAL::getAcademicYearByDate(const QDate &start_date, const QDate &end_date)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare("SELECT * FROM academic_year WHERE start_date = :start AND end_date = :end LIMIT 1 ");

    query.bindValue(":start", start_date);
    query.bindValue(":end", end_date);

    if (!query.exec()) {
        qWarning() << "GET ACADEMIC YEAR BY DATE ERROR:" << query.lastError().text();
        return std::nullopt;
    }

    if (query.next()) {
        DataModel::AcademicYear year;
        year.year_id    = query.value("year_id").toInt();
        year.name       = query.value("name").toString();
        year.start_date = query.value("start_date").toDate();
        year.end_date   = query.value("end_date").toDate();
        year.is_active  = query.value("is_active").toBool();
        return year;
    }

    return std::nullopt;
}

std::optional<DataModel::AcademicYear> DataAccess::AcademicYearDAL::getUpcomingAcademicYear()
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare("SELECT * FROM academic_year WHERE start_date > CURRENT_DATE ORDER BY start_date ASC LIMIT 1 ");

    if(!query.exec())
    {
        qWarning()<<"GET UPCONING ACADEMIC YEAR ERROR:" <<query.lastError().text();
        return std::nullopt;
    }


    if (query.next()) {
        DataModel::AcademicYear year;
        year.year_id    = query.value("year_id").toInt();
        year.name       = query.value("name").toString();
        year.start_date = query.value("start_date").toDate();
        year.end_date   = query.value("end_date").toDate();
        year.is_active  = query.value("is_active").toBool();
        return year;
    }

    return std::nullopt;
}

std::optional<DataModel::AcademicYear> DataAccess::AcademicYearDAL::getPreviousAcademicYear()
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare(R"(
    SELECT *
    FROM academic_year
    WHERE end_date < CURRENT_DATE
    ORDER BY end_date DESC
    LIMIT 1
)");


    if(!query.exec())
    {
        qWarning()<<"GET PREVIOUS ACADEMIC YEAR ERROR:" <<query.lastError().text();
        return std::nullopt;
    }

    if (query.next()) {
        DataModel::AcademicYear year;
        year.year_id    = query.value("year_id").toInt();
        year.name       = query.value("name").toString();
        year.start_date = query.value("start_date").toDate();
        year.end_date   = query.value("end_date").toDate();
        year.is_active  = query.value("is_active").toBool();
        return year;
    }

    return std::nullopt;

}

bool DataAccess::AcademicYearDAL::isYearActive(int id)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare("SELECT EXISTS (SELECT 1 FROM academic_year WHERE year_id = :id AND is_active = TRUE)" );
    query.bindValue(":id", id);

    if (!query.exec()) {
        qWarning() << "IS ACADEMIC YEAR ACTIVE ERROR:" << query.lastError().text();
        return false;
    }

    if (query.next())
        return query.value(0).toBool();

    return false;
}

double DataAccess::AcademicYearDAL::getYearProgress(int yearId)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare("SELECT start_date, end_date FROM academic_year WHERE year_id = :id LIMIT 1 ");
    query.bindValue(":id", yearId);
    if (!query.exec())
    {
        qWarning() << "GET YEAR PROGRESS ERROR:" << query.lastError().text();
        return 0.0;
    }

    if(query.next())
    {
        QDate start = query.value("start_date").toDate();
        QDate end = query.value("end_date").toDate();
        QDate today = QDate::currentDate();


        if(!start.isValid() || end.isValid() || start >= end )
            return 0.0;

        int totalDays = start.daysTo(end);
        int elapsedDays = start.daysTo(today);
        if(elapsedDays < 0) return 0.0;
        if(elapsedDays >= totalDays) return 100;

        return(static_cast<double>(elapsedDays) / totalDays) * 100.0;

    }

    return 0.0;
}

int DataAccess::AcademicYearDAL::getRemainingDays(int yearId)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare(R"(
        SELECT end_date
        FROM academic_year
        WHERE year_id = :id
        LIMIT 1
    )");
    query.bindValue(":id", yearId);

    if (!query.exec())
    {
        qWarning() << "GET REMAINING DAYS ERROR:" << query.lastError().text();
        return 0;
    }

    if (query.next())
    {
        QDate end   = query.value("end_date").toDate();
        QDate today = QDate::currentDate();

        if (!end.isValid()) return 0;

        int remaining = today.daysTo(end);
        return remaining > 0 ? remaining : 0;
    }

    return 0;
}








