#include "AcademicYearDataHandler.h"
#include"DatabaseManager/databaseManager.h"
#include"Logger.h"


#include<QSqlQuery>
#include<QSqlDatabase>
#include<QSqlError>


bool DataAccess::AcademicYearDataHandler::addAcademicYear(const DataModel::AcademicYear &year)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    if(!db.isOpen())
    {
        qCritical() <<"\033[31m Database connection is not open \033[0m " + db.lastError().text();
        Logger::instance().error("Database connection is not open " + db.lastError().text());
        return false;
    }

    if(!db.transaction())
    {
        qCritical()<<"\033[31m Cannot start transaction \033[0m" + db.lastError().text();
        Logger::instance().error("Cannot start transaction "+ db.lastError().text());
        return false;
    }

    QSqlQuery query(db);
    query.prepare(R"(
                    INSERT INTO academic_year
                            (name, start_date, end_date, is_active, holidays_count)
                    VALUES
                            (:name, :start, :end, :is_active, :holidays)
                    RETURNING year_id
    )");


    query.bindValue(":name", year.name.trimmed());
    query.bindValue(":start", year.start_date);
    query.bindValue(":end", year.end_date);
    query.bindValue(":is_active", year.is_active);
    query.bindValue(":holidays", year.holidays_count);

    if(!query.exec())
    {
        qCritical() << "\033[31m Failed to add academic year\033[0m" + query.lastError().text();
        Logger::instance().error("Failed to add academic year" + query.lastError().text());
        db.rollback();
        return false;
    }


    if(query.next())
    {
        int newId = query.value(0).toInt();
        qDebug() <<"\033[32m Academic year addes successfully with ID: " + QString::number(newId);
    }


    if(!db.commit())
    {
        qCritical() <<"\033[31m Failed to commit transaction \033[0m" << db.lastError().text();
        Logger::instance().error("Failed to commit transaction " + db.lastError().text());
        return false;
    }

    return true;

}

std::optional<DataModel::AcademicYear> DataAccess::AcademicYearDataHandler::getAcademicYearById(int yearId)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    if(!db.isOpen())
    {
        qCritical() <<"\033[31m Database connection is not open \033[0m " + db.lastError().text();
        Logger::instance().error("Database connection is not open " + db.lastError().text());
        return std::nullopt;
    }


    QSqlQuery query(db);
    query.setForwardOnly(true);
    query.prepare(R"(
                    SELECT year_id, name, start_date, end_date, is_active, holidays_count
                    FROM academic_year
                    WHERE year_id = ?
    )");

    query.addBindValue(yearId);

    if(!query.exec())
    {
        qCritical()  << "\033[31m Failed to get academic year \033[0m "+ query.lastError().text();
        Logger::instance().error("Failed to get academic year " + query.lastError().text());
        return std::nullopt;
    }

    if(!query.next())
    {
        qDebug() <<"\033[33m No academic year found with ID: \033[0m " + query.lastError().text();
        Logger::instance().error("No academic year found with ID: " + QString::number(yearId) + query.lastError().text());
        return std::nullopt;
    }

    try
    {
        DataModel::AcademicYear academicYear;
        academicYear.year_id = query.value("year_id").toInt();
        academicYear.name = query.value("name").toString();
        academicYear.start_date = query.value("start_date").toDate();
        academicYear.end_date = query.value("end_date").toDate();
        academicYear.is_active = query.value("is_active").toBool();
        academicYear.holidays_count = query.value("holidays_count").toInt();

        qDebug() <<"\033[32m Successfully retrieved academic year: " + academicYear.name;
        Logger::instance().debug("Successfully retrieved academic year: " + academicYear.name);

        return academicYear;
    }
    catch(const std::exception& e)
    {
        qCritical() <<"\033[31m Error processing academic year data: \033[0m" <<e.what();
        Logger::instance().error("Error processing academic year data: ");
        return std::nullopt;
    }
    return std::nullopt;
}

QVector<DataModel::AcademicYear> DataAccess::AcademicYearDataHandler::getAllAcademicYears()
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    if(!db.isOpen())
    {
        qCritical() <<"\033[31m Database connection is not open \033[0m " + db.lastError().text();
        Logger::instance().error("Database connection is not open " + db.lastError().text());
        return QVector<DataModel::AcademicYear>();
    }

    QSqlQuery query(db);
    query.setForwardOnly(true);
    query.prepare(R"(
                SELECT year_id, name, start_date, end_date, is_active, holidays_count
                FROM academic_year
                ORDER BY start_date DESC, year_id DESC
    )");

    if(!query.exec())
    {
        qCritical() <<"\033[31m Failed to get academic yaers \033[0m" + query.lastError().text();
        Logger::instance().error("Failed to get academic years " + query.lastError().text());
        return QVector<DataModel::AcademicYear>();
    }

    QVector<DataModel::AcademicYear> academicYears;
    int resultCount = 0;
    while(query.next())
    {
        try{
        DataModel::AcademicYear academicYear;

        academicYear.year_id = query.value("year_id").toInt();
        academicYear.name = query.value("name").toString();
        academicYear.start_date = query.value("start_date").toDate();
        academicYear.end_date = query.value("end_date").toDate();
        academicYear.is_active = query.value("is_active").toBool();
        academicYear.holidays_count = query.value("holidays_count").toInt();

        academicYears.append(academicYear);
        resultCount++;
    }
    catch(const std::exception& e)
    {
            qCritical() <<"\033[31m Error processing academic year data: \033[0m" <<e.what();
        Logger::instance().error("Error processing academic year data: " + QString(e.what()));
    }
    }

    qDebug() <<"\033[32m Successfully retrieved \033[0m" << resultCount << "\033[32m academic years \033[0m";

    return academicYears;
}

bool DataAccess::AcademicYearDataHandler::updateAcademicYear(const DataModel::AcademicYear &year)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    if(!db.isOpen())
    {
        qCritical() <<"\033[31m Database connection is not open \033[0m " + db.lastError().text();
        Logger::instance().error("Database connection is not open " + db.lastError().text());
        return false;
    }

    if(!db.transaction())
    {
        qCritical() <<"\033[31m Cannot start transaction \033[0m"+ db.lastError().text();
        Logger::instance().error("Cannot start transaction "+ db.lastError().text());
        return false;
    }

    QStringList updateFields;
    QVector<QPair<QString, QVariant>> bindValues;

    if(!year.name.isEmpty())
    {
        updateFields <<"name = :name";
        bindValues << qMakePair(":name",year.name.trimmed());
    }

    if(year.start_date.isValid())
    {
        updateFields << "start_date = :start";
        bindValues << qMakePair(":start",year.start_date);
    }

    if(year.end_date.isValid())
    {
        updateFields <<"end_date = :end";
        bindValues << qMakePair(":end",year.end_date);
    }


        updateFields << "is_active = :is_active";
        bindValues << qMakePair(":is_active",year.is_active);

        updateFields << "holidays_count = :holidays";
        bindValues << qMakePair(":holidays",year.holidays_count);
        if(updateFields.isEmpty())
        {
            db.rollback();
            qWarning() << "\033[33m No fields to update \033[0m";
            Logger::instance().warning("No fields to update for academic year ID: " + QString::number(year.year_id));
            return false;
        }

        bindValues  <<qMakePair(":year_id", year.year_id);

        QString queryString = QString("UPDATE academic_year SET %1 WHERE year_id = :year_id").arg(updateFields.join(", "));

        QSqlQuery query(db);
        if (!query.prepare(queryString))
        {
            qCritical() << "\033[31m Failed to prepare query \033[0m" << query.lastError().text();
            Logger::instance().error("Failed to prepare query: " + query.lastError().text());
            db.rollback();
            return false;
        }

        // bind values
        for(const auto& Pair : bindValues)
            query.bindValue(Pair.first, Pair.second);

        if(!query.exec())
        {
            qCritical() << "\033[31m Failed to update academic year \033[0m" << query.lastError().text();
            Logger::instance().error("Failed to update academic year: " + query.lastError().text());
            db.rollback();
            return false;
        }

        if(query.numRowsAffected() <= 0)
        {
            qWarning() << "\033[33m No academic year found with ID: \033[0m" << year.year_id;
            Logger::instance().warning("No academic year found with ID: " + QString::number(year.year_id));
            db.rollback();
            return false;
        }


        if(year.is_active)
        {
            QSqlQuery deactivateQuery(db);
            deactivateQuery.prepare("UPDATE academic_year SET is_active = false WHERE year_id != ?");
            deactivateQuery.addBindValue(year.year_id);


            if(!deactivateQuery.exec())
            {
                qWarning() << "\033[33m Failed to deactivate other academic years \033[0m" << deactivateQuery.lastError().text();
                Logger::instance().warning("Failed to deactivate other academic years: " + deactivateQuery.lastError().text());

                // go on without stop, just log what happen
            }
        }

        if(!db.commit())
        {
            qCritical() <<"\033[31m Failed to commit transaction: \033[0m" + db.lastError().text();
            Logger::instance().error("Failed to commit transaction: " + db.lastError().text());
            db.rollback();
            return false;
        }

        qDebug() <<"\033[32m Successfully updated academic year ID: \033[0m" << QString::number(year.year_id);
        return true;
}

bool DataAccess::AcademicYearDataHandler::deleteAcademicYear(int yearId)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    if(!db.isOpen())
    {
        qCritical() <<"\033[31m Database connection is not open \033[0m " + db.lastError().text();
        Logger::instance().error("Database connection is not open " + db.lastError().text());
        return false;
    }

    if(!db.transaction())
    {
        qCritical() <<"\033[31m Cannot start transaction \033[0m"+ db.lastError().text();
        Logger::instance().error("Cannot start transaction "+ db.lastError().text());
        return false;
    }


    QSqlQuery query(db);
    query.prepare("DELETE FROM academic_year WHEER year_id = ?")   ;
    query.addBindValue(yearId);

    if(!query.exec())
    {
        qCritical()  << "\033[31m Failed to delete academic year \033[0m "+ query.lastError().text();
        Logger::instance().error("Failed to delete academic year " + query.lastError().text());
        return false;
    }

    if(!db.commit())
    {
        qCritical() <<"\033[31m Failed to commit transaction \033[0m" << db.lastError().text();
        Logger::instance().error("Failed to commit transaction " + db.lastError().text());
        return false;
    }

    qDebug() << "\033[32m Deleted academic year successfully . \033[0m";
    return true;
}

std::optional<DataModel::AcademicYear> DataAccess::AcademicYearDataHandler::getCurrentAcademicYear()
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.setForwardOnly(true);
    query.prepare("SELECT year_id, name, start_date, end_date, is_active, holidays_count FROM academic_year WHERE CURRENT_DATE >= start_date AND CURRENT_DATE <= end_date  LIMIT 1");



    if(query.exec() && query.next())
    {
        DataModel::AcademicYear ay;

        ay.year_id = query.value("year_id").toInt();
        ay.name = query.value("name").toString();
        ay.start_date = query.value("start_date").toDate();
        ay.end_date = query.value("end_date").toDate();
        ay.is_active = query.value("is_active").toBool();
        ay.holidays_count = query.value("holidays_count").toInt();

        return ay;
    }

    qWarning() << "\033[33m There are no result to return \033[0m";
    return std::nullopt;
}

bool DataAccess::AcademicYearDataHandler::setAcademicYearActive(int yearId)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    if(!db.isOpen())
    {
        qCritical() <<"\033[31m Database connection is not open \033[0m " + db.lastError().text();
        Logger::instance().error("Database connection is not open " + db.lastError().text());
        return false;
    }

    if(!db.transaction())
    {
        qCritical() <<"\033[31m Cannot start transaction \033[0m"+ db.lastError().text();
        Logger::instance().error("Cannot start transaction "+ db.lastError().text());
        return false;
    }



    QSqlQuery query(db);
    query.prepare("UPDATE academic_year SET is_active = true WHERE year_id = :id");

    if(!query.exec())
    {
        qCritical() << "\033[33m Failed to activate academic year \033[0m " + query.lastError().text();
        Logger::instance().error("Failed to activate academic year " +query.lastError().text());
        db.rollback();
        return false;
    }




    QSqlQuery deactivateQuery(db);
    deactivateQuery.prepare("UPDATE academic_year SET is_active = false WHERE year_id != ?");
    deactivateQuery.addBindValue(yearId);


    if(!deactivateQuery.exec())
    {
        qWarning() << "\033[33m Failed to deactivate other academic years \033[0m" << deactivateQuery.lastError().text();
        Logger::instance().warning("Failed to deactivate other academic years: " + deactivateQuery.lastError().text());

        // go on without stop, just log what happen
    }

    if(!db.commit())
    {
        qCritical() <<"\033[31m Failed to commit transaction: \033[0m" + db.lastError().text();
        Logger::instance().error("Failed to commit transaction: " + db.lastError().text());
        db.rollback();
        return false;
    }

    qDebug() <<"\033[32m Successfully activate  academic year ID: \033[0m" << QString::number(yearId);
    return true;

}

bool DataAccess::AcademicYearDataHandler::isAcademicYearActive(int yearId)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    QSqlQuery query(db);
    query.setForwardOnly(true);
    query.prepare(R"( SELECT is_active FROM academic_year WHERE year_id = :id LIMIT 1)");
    query.bindValue(":id", yearId);

    if(!query.exec())
    {
        qCritical() << "\033[33m Failed to check academic year is active \033[0m " + query.lastError().text();
        Logger::instance().error("Failed to check academic year is active " +query.lastError().text());
        return false;
    }
    if(query.next())
        return query.value(0).toBool();

    return false;
}

QVector<DataModel::AcademicYear> DataAccess::AcademicYearDataHandler::getAcademicYearByDateRange(const QDate &start, const QDate &end)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    QSqlQuery query(db);
    query.setForwardOnly(true);
    query.prepare("SELECT year_id, name, start_date, end_date, is_active, holidays_count FROM academic_year WHERE start_date AND end_date BETWEEN :start AND :end");

    if(!query.exec())
    {
        qCritical() <<"\033[33m Failed to get acadmeic year in range " + query.lastError().text();
        Logger::instance().error("Failed to get acadmeic year in range " + query.lastError().text());

        return QVector<DataModel::AcademicYear>();
    }
    QVector<DataModel::AcademicYear> academicYears;
    while(query.next())
    {
        DataModel::AcademicYear ay;
        ay.year_id = query.value("year_id").toInt();
        ay.name = query.value("name").toString();
        ay.start_date = query.value("start_date").toDate();
        ay.end_date = query.value("end_date").toDate();
        ay.is_active = query.value("is_active").toBool();
        ay.holidays_count = query.value("holidays_count").toInt();
        academicYears.append(ay);
    }
    return academicYears;

}

// int DataAccess::AcademicYearDataHandler::getAcademicYearCount()
// {
//     auto connWrapper = DatabaseManager::instance().getConnection();
//     QSqlDatabase db = connWrapper->database();

//     QSqlQuery query(db);
//     query.setForwardOnly(true);
//     query.prepare("SELECT COUNT(year_id) FROM academic_year");

// }




















































