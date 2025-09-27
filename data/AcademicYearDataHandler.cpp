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
        qCritical() << "\033[31m Failed to add academic year \033[0m " + query.lastError().text();
        Logger::instance().error("Failed to add academic year" + query.lastError().text());
        db.rollback();
        return false;
    }


    if(query.next())
    {
        int newId = query.value(0).toInt();
        qDebug() <<"\033[32m Academic year addes successfully with ID: \033[0m" + QString::number(newId);
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
    query.bindValue(":id", yearId);

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
    if(!db.isOpen())
    {
        qCritical() <<"\033[31m Database connection is not open \033[0m " + db.lastError().text();
        Logger::instance().error("Database connection is not open " + db.lastError().text());
        return false;
    }
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
    if(!db.isOpen())
    {
        qCritical() <<"\033[31m Database connection is not open \033[0m " + db.lastError().text();
        Logger::instance().error("Database connection is not open " + db.lastError().text());
        return QVector<DataModel::AcademicYear>();
    }
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

std::optional<int> DataAccess::AcademicYearDataHandler::getAcademicYearCount()
{
 const   auto& connWrapper = DatabaseManager::instance().getConnection();
  const  QSqlDatabase& db = connWrapper->database();
 if(!db.isOpen())
 {
     qCritical() <<"\033[31m Database connection is not open \033[0m " + db.lastError().text();
     Logger::instance().error("Database connection is not open " + db.lastError().text());
     return false;
 }
    QSqlQuery query(db);
    query.setForwardOnly(true);
    query.prepare("SELECT COUNT(year_id) FROM academic_year");

    if(!query.exec())
    {
        qCritical() << "\033[31m Failed to execute get all academic year query \033[0m" << query.lastError().text();
        Logger::instance().error("Failed to execute get all academic year query "+ query.lastError().text());
         return std::nullopt;
    }

    if(query.next())
    {
        qDebug() << "\033[32m The total count of academic year: " << QString::number(query.value(0).toInt());
        return query.value(0).toInt();
    }


    qWarning() << "\033[33m Some thing wrong: \033[0m" << query.lastError().text();
    Logger::instance().warning("Some thing wrong: " + query.lastError().text());
    return std::nullopt;

}

std::optional<int>  DataAccess::AcademicYearDataHandler::getActiveAcademicYearId()
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    const QSqlDatabase& db = connWrapper->database();
    if(!db.isOpen())
    {
        qCritical() <<"\033[31m Database connection is not open \033[0m " + db.lastError().text();
        Logger::instance().error("Database connection is not open " + db.lastError().text());
        return false;
    }
    QSqlQuery query(db);
    query.setForwardOnly(true);

    query.prepare(R"(
                SELECT year_id FROM academic_year WHERE is_active = true LIMIT 1
)");


    if(!query.exec())
    {
        qCritical() << "\033[31m Failed to execute get  academic year id query \033[0m" << query.lastError().text();
        Logger::instance().error("Failed to execute get academic year id query "+ query.lastError().text());
        return std::nullopt;
    }

    if(query.next())
    {
        qDebug() << "\033[32m The  active academic year ID: " << QString::number(query.value(0).toInt());
        return query.value(0).toInt();
    }


    qWarning() << "\033[33m Some thing wrong: \033[0m" << query.lastError().text();
    Logger::instance().warning("Some thing wrong: " + query.lastError().text());
    return std::nullopt;
}

bool DataAccess::AcademicYearDataHandler::isAcademicYearExist(int yearId)
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    const QSqlDatabase& db = connWrapper->database();
    if(!db.isOpen())
    {
        qCritical() <<"\033[31m Database connection is not open \033[0m " + db.lastError().text();
        Logger::instance().error("Database connection is not open " + db.lastError().text());
        return false;
    }
    QSqlQuery query(db);
    query.setForwardOnly(true);
    query.prepare("SELECT EXISTS(SELECT 1 FROM academic_year WHERE year_id = :id)");
    query.bindValue(":id", yearId);

    if(!query.exec())
    {
        qCritical() << "\033[31m Failed to execute is Academic academic year exist \033[0m" << query.lastError().text();
        Logger::instance().error("Failed to execute is Academic academic year exist "+ query.lastError().text());
        return false;
    }

    if(query.next())
    {
        qDebug() <<"\033[32m result returned successfully. \033[0m";
        return query.value(0).toBool();
    }
    qWarning() << "\033[33m Some thing wrong: \033[0m" << query.lastError().text();
    Logger::instance().warning("Some thing wrong: " + query.lastError().text());
    return false;
}

bool DataAccess::AcademicYearDataHandler::isAcademicYearNameExist(const QString &name)
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    const QSqlDatabase& db = connWrapper->database();
    if(!db.isOpen())
    {
        qCritical() <<"\033[31m Database connection is not open \033[0m " + db.lastError().text();
        Logger::instance().error("Database connection is not open " + db.lastError().text());
        return false;
    }
    QSqlQuery query(db);
    query.setForwardOnly(true);
    query.prepare("SELECT EXISTS(SELECT 1 FROM academic_year WHERE name = :name)");
    query.bindValue(":name", name);

    if(!query.exec())
    {
        qCritical() << "\033[31m Failed to execute is Academic academic year exist \033[0m" << query.lastError().text();
        Logger::instance().error("Failed to execute is Academic academic year exist "+ query.lastError().text());
        return false;
    }

    if(query.next())
    {
        qDebug() <<"\033[32m result returned successfully. \033[0m";
        return query.value(0).toBool();
    }
    qWarning() << "\033[33m Some thing wrong: \033[0m" << query.lastError().text();
    Logger::instance().warning("Some thing wrong: " + query.lastError().text());
    return false;
}

bool DataAccess::AcademicYearDataHandler::deactivateAllAcademicYears()
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
    query.prepare("UPDATE academic_year SET is_active = false");
    if(!query.exec())
    {
        qCritical() << "\033[31m Failed to execute deactivate all academic years \033[0m" << query.lastError().text();
        Logger::instance().error("Failed to execute deactivate all academic years "+ query.lastError().text());
        db.rollback();
        return false;
    }

    if(!db.commit())
    {
        qCritical() <<"\033[31m Failed to commit transaction \033[0m" << db.lastError().text();
        Logger::instance().error("Failed to commit transaction " + db.lastError().text());
        return false;
    }

    return query.numRowsAffected() > 0;
}

int DataAccess::AcademicYearDataHandler::getHolidaysCount(int yearId)
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    const QSqlDatabase& db = connWrapper->database();

    if(!db.isOpen())
    {
        qCritical() <<"\033[31m Database connection is not open \033[0m " + db.lastError().text();
        Logger::instance().error("Database connection is not open " + db.lastError().text());
        return false;
    }


    QSqlQuery query(db);
    query.prepare("SELECT holidays_count FROM academic_year  WHERE year_id = ?");
    query.addBindValue(yearId);

    if (!query.exec())
    {
        qCritical() << "\033[31m Failed to get holidays count \033[0m" << query.lastError().text();
        Logger::instance().error("Failed to get holidays count: " + query.lastError().text());
        return 0;
    }

    if(query.next())
        return query.value(0).toInt();

    qWarning() <<"\033[33m Some thing wrong \033[0m" << query.lastError().text();
    Logger::instance().error("Some thing wrong "+ query.lastError().text());
    return 0;



}



bool DataAccess::AcademicYearDataHandler::updateHolidaysCount(int yearId, int holidaysCount)
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    const QSqlDatabase& db = connWrapper->database();

    if(!db.isOpen())
    {
        qCritical() <<"\033[31m Database connection is not open \033[0m " + db.lastError().text();
        Logger::instance().error("Database connection is not open " + db.lastError().text());
        return false;
    }


    QSqlQuery query(db);
    query.prepare("UPDATE academic_year SET holidays_count = ? WHERE year_id = ?");
    query.addBindValue(holidaysCount);
    query.addBindValue(yearId);

    if(!query.exec())
    {
        qCritical() << "\033[31m Failed to execute update holidays count in academic year \033[0m" << query.lastError().text();
        Logger::instance().error("Failed to execute update holidays count in academic year "+ query.lastError().text());
        return false;
    }

    qDebug() <<"\033[32m Uppdated holidays count successfully. \033[0m";
    return query.numRowsAffected() > 0;

}

int DataAccess::AcademicYearDataHandler::getRemainingHolidays(int yearId)
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    const QSqlDatabase& db = connWrapper->database();
    if (!db.isOpen())
    {
        qCritical() << "\033[31m Database connection is not open \033[0m";
        Logger::instance().error("Database connection is not open");
        return 0;
    }

    int totalHolidays = AcademicYearDataHandler::getHolidaysCount(yearId);


    QSqlQuery query(db);
    query.prepare(R"(
                    SELECT COUNT(*) AS used_holidays
                    FROM holidays
                    WHERE year_id = ?
                    AND is_palanned = true
    )");

    query.addBindValue(yearId);

    if(!query.exec())
    {
        qCritical() << "\033[31m Failed to get used holidays count \033[0m" << query.lastError().text();
        Logger::instance().error("Failed to get used holidays count: " + query.lastError().text());
        return totalHolidays;
    }

    int usedHolidays = 0;
    if(query.next())
        usedHolidays = query.value("used_holidays").toInt();

    int remainingHolidays = totalHolidays - usedHolidays;


    if(remainingHolidays < 0)
        remainingHolidays =0;


    qDebug() << "\033[32m Remaining holidays for year ID \033[0m" << yearId
             <<": " << remainingHolidays << "/" << totalHolidays;

    return remainingHolidays;
}

bool DataAccess::AcademicYearDataHandler::isDateInAcademicYear(int yearId, const QDate &date)
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    const QSqlDatabase& db = connWrapper->database();

    if(!db.isOpen())
    {
        qCritical() <<"\033[31m Database connection is not open \033[0m" + db.lastError().text();
        Logger::instance().error("Database connection is not open " + db.lastError().text());
        return false;
    }

    QSqlQuery query(db);
    query.prepare(R"(
                    SELECT EXISTS(SELECT 1 FROM academic_year
                    WHERE year_id = ? AND ? BETWEEN start_date AND end_date)
                )");

    query.addBindValue(yearId);
    query.addBindValue(date);

    return query.exec() && query.next() && query.value(0).toBool();
}

int DataAccess::AcademicYearDataHandler::getDaysElapsedInAcademicYear(int yearId)
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    const QSqlDatabase& db = connWrapper->database();

    if(!db.isOpen())
    {
        qCritical() <<"\033[31m Database connection is not open \033[0m" << db.lastError().text();
        Logger::instance().error("Database connection is not open" + db.lastError().text());
        return 0;
    }

    QSqlQuery query(db);
    query.setForwardOnly(true);
    query.prepare(R"(
            SELECT start_date, end_date
            FROM academic_year
            WHERE year_id = ?
    )");

    query.addBindValue(yearId);

    if(!query.exec())
    {
        qCritical() <<"\033[31m Failed to get academic year dates \033[0m" <<query.lastError().text();
        Logger::instance().error("Failed to get academic year dates " + query.lastError().text());
        return 0;
    }

    if(!query.next())
    {
        qWarning() <<"\033[33m No academic year found with ID: \033[0m" << yearId;
        Logger::instance().warning("No academic year found with ID: " + QString::number(yearId));
        return 0;
    }

    QDate start = query.value("start_date").toDate();
    QDate end = query.value("end_date").toDate();
    QDate currentDate =  QDate::currentDate();

    if(currentDate < start)
    {
        qDebug() <<"\033[33m Academic year has not started yet. \033[0m";
        Logger::instance().debug("Academic year has not started yet. ");
        return 0;
    }

    if(currentDate > end)
    {
        int totalDays = start.daysTo(end) + 1;
        qDebug() << "\033[36m Academic year has ended. Total days: \033[0m" <<totalDays;
        return totalDays;
    }

    int daysElapsed = start.daysTo(currentDate) + 1;

    qDebug() <<"033[32m Days elapsed in academic year ID \033[0m" <<yearId << ": " << daysElapsed;
    return daysElapsed;

}

int DataAccess::AcademicYearDataHandler::getDaysRemainingInAcademicYear(int yearId)
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    const QSqlDatabase& db = connWrapper->database();
    if(!db.isOpen())
    {
        qCritical() <<"\033[31m Database connection is not open." << db.lastError().text();
        Logger::instance().error("Database connection is not open" + db.lastError().text());
        return 0;
    }
    QSqlQuery query(db);
    query.setForwardOnly(true);

    query.prepare(R"(
            SELECT end_date,
                CASE
                    WHEN CURRENT_DATE > end_date THEN 0
                    ELSE end_date - CURRENT_DATE
                END AS remaining_days
            FROM academic_year
            WHERE year_id = ?
)");
    query.addBindValue(yearId);

    if(!query.exec())
    {
        qCritical() <<"\033[31m Failed to get remaining days in academic year ID: \033[0m" <<yearId ;
        Logger::instance().error("Failed to get remaining days in academic year ID: " + QString::number(yearId));
        return 0;
    }

    if(query.next())
    {
            int        remainingDays   = query.value("remaining_days").toInt();
        return (remainingDays > 0)  ? remainingDays : 0;
    }
    return 0;
}


double DataAccess::AcademicYearDataHandler::getAcademicYearProgress(int yearId)
{
    int daysElapsed = getDaysElapsedInAcademicYear(yearId);
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    const QSqlDatabase& db = connWrapper->database();

    QSqlQuery query(db);
    query.setForwardOnly(true);
    query.prepare("SELECT start_date, end_date FROM academic_year WHERE year_id = ?");
    query.addBindValue(yearId);
    if(query.exec() && query.next())
    {
        QDate  start = query.value("start_date").toDate();
        QDate  end = query.value("end_date").toDate();

        int totalDays =start.daysTo(end) + 1;
        if(totalDays > 0)
        {
            double progress = (static_cast<double>(daysElapsed) / totalDays) * 100.0;
            return qMin(progress, 100.0);
        }

    }

    return 0.0;

}

int DataAccess::AcademicYearDataHandler::getAcademicYearDuration(int yearId)
{
    const auto &connWrapper = DatabaseManager::instance().getConnection();
    const QSqlDatabase& db = connWrapper->database();

    if(!db.isOpen())
    {
        qCritical() <<"\033[31m Database connection is not open \033[0m" << db.lastError().text();
        Logger::instance().error("Database connection is not open" + db.lastError().text());
        return 0;
    }

    QSqlQuery query(db);
    query.prepare(R"(
            SELECT start_date, end_date
            FROM academic_year
            WHERE year_id = ?
)");
    query.addBindValue(yearId);


    if(!query.exec())
    {
        qCritical() << "\033[31m Failed to get academic year duration \033[0m" << query.lastError().text();
        Logger::instance().error("Failed to get academic year duration" + query.lastError().text());
        return 0;
    }

    if(!query.next())
        return 0;
    QDate start = query.value("start_date").toDate();
    QDate end = query.value("end_date").toDate();

    if(start > end)
    {
        qWarning() <<"\033[33m start date is after end date \033[0m";
        return 0;
    }

    int duration = start.daysTo(end) + 1;
    qDebug() <<"\033[32m Academic year duration for ID: " <<yearId << ": " << duration << "days\n" << start.toString("yyyy-MM-dd") + " to " + end.toString("yyyy-MM-dd");
    return duration;

}

bool DataAccess::AcademicYearDataHandler::addHolidayToAcademicYear(int yearId, const DataModel::Holiday &holiday)
{
    const auto& connWrapper  = DatabaseManager::instance().getConnection();
     QSqlDatabase db = connWrapper->database();

    if(!db.isOpen())
    {
        qCritical() <<"\033[31m Database connection is not open \033[0m" << db.lastError().text();
        Logger::instance().error("Database connection is not open" + db.lastError().text());
        return false;
    }

    if(!db.transaction())
    {
        qCritical() <<"\033[31m Failed to start transaction \033[0m" + db.lastError().text();
        Logger::instance().error("Failed to start transaction " + db.lastError().text());
        return false;
    }

    QSqlQuery query(db);
    query.prepare(R"(
            INSERT INTO holidays (year_id, name, start_date, end_date, type, is_planned)
            VALUES (?, ?, ?, ?, ?, ?)
            RETURNING holiday_id
    )");



    query.addBindValue(yearId);
    query.addBindValue(holiday.name.trimmed());
    query.addBindValue(holiday.start_date);
    query.addBindValue(holiday.end_date);
    query.addBindValue(static_cast<int>(holiday.type));
    query.addBindValue(holiday.is_planned);


    if(!query.exec() || !query.next())
    {
        qCritical() <<"\033[33m Failed to execution \033[0m" + query.lastError().text();
        Logger::instance().error("Failed to execution " + query.lastError().text());
        db.rollback();
        return false;
    }

    if(!db.commit())
    {
        qCritical() <<"\033[31m Failed to commit " + db.lastError().text();
        Logger::instance().error("Failed to commit " + db.lastError().text());
        db.rollback();
        return false;
    }

    return true;
}

QVector<DataModel::Holiday> DataAccess::AcademicYearDataHandler::getHolidaysByAcademicYear(int yearId)
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    const QSqlDatabase& db = connWrapper->database();

    if(!db.isOpen())
    {
        qCritical() <<"\033[31m Database connection is not open " + db.lastError().text();
        Logger::instance().error("Database connection is not open " + db.lastError().text());
        return QVector<DataModel::Holiday>();
    }

    QSqlQuery query(db);
    query.setForwardOnly(true);
    query.prepare(R"(
                SELECT holidays_id, name, start_date, end_date, type, is_planned
                FROM holidays
                WHERE year_id = ?
                ORDER BY start_date ASC
)");

    query.addBindValue(yearId);


    if(!query.exec())
    {
        qCritical() << "\033[31m Failed to get holidays for academic year ID: \033[0m" <<yearId <<" " + query.lastError().text();
        Logger::instance().error("Failed to get holidays for academic year ID: " +  QString::number(yearId) + " " +query.lastError().text());
        return QVector<DataModel::Holiday>();
    }
    QVector<DataModel::Holiday> holidays;
    while(query.next())
    {
        DataModel::Holiday h;
        h.holiday_id = query.value("holiday_id").toInt();
        h.name = query.value("name").toString();
        h.start_date = query.value("start_date").toDate();
        h.end_date = query.value("end_date").toDate();
        h.type = DataModel::intToHolidayType(query.value("type").toInt());
        h.is_planned = query.value("is_palnned").toBool();
        holidays.append(h);
    }

    qDebug() <<"\033[32m Retrieved "<<holidays.size() << " holidays for academic year ID: \033[0m " <<yearId;
    return holidays;
}

bool DataAccess::AcademicYearDataHandler::closeAcademicYear(int yearId)
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    if(!db.isOpen())
    {
        qCritical() << "\033[31m Database connecton is not open " << db.lastError().text();
        Logger::instance().error("Databse connection is not open "  + db.lastError().text());
        return false;
    }

    if(!db.transaction())
    {
        qCritical() <<"\033[31m Failed to start transaction " + db.lastError().text();
        Logger::instance().error("Failed to start transaction " + db.lastError().text());
        return false;
    }

    try
    {
        QSqlQuery checkQuery(db);
        checkQuery.prepare("SELECT yesr_id, is_active FROM academic_year WHERE year_id = ? LIMIT 1");
        checkQuery.addBindValue(yearId);

        if(!checkQuery.exec() || !checkQuery.next())
        {
            qCritical() <<"\033[31m Academic year not found with ID: " + yearId;
            Logger::instance().error("Academic year noy found with ID: " + QString::number(yearId));
            db.rollback();
            return false;
        }

        bool isCurrentlyActive = checkQuery.value("is_active").toBool();
        if(!isCurrentlyActive)
        {
            qWarning() << "\033[33m Academic year is already closed for ID: \033[0m" << yearId;
            Logger::instance().warning("Academic year is already closed for ID: " + QString::number(yearId));
            db.rollback();
            return true;
        }


//         QSqlQuery activeStudentsQuery(db);
//         activeStudentsQuery.prepare(R"(
//                 SELECT COUNT(*) AS active_count
//                 FROM student_enrollment se
//                 WHERE se.year_id = ? AND se.status = 1
// )");
//     activeStudentsQuery.addBindValue(yearId);

//     if(activeStudentsQuery.exec() && activeStudentsQuery.next())
//     {
//         int activeCount =activeStudentsQuery.value("active_count").toInt();
//         if(activeCount > 0)
//         {
//             qCritical() <<"\033[31m Cannot close academic year: There are " << activeCount <<" active students enrolled. \033[0m";
//             Logger::instance().error("Cannot close academic year: There are " + QString::number(activeCount) + " active students enrolled.");
//             db.rollback();
//             return false;
//         }
//     }


    QSqlQuery dateQuery(db);
    dateQuery.prepare("SELECT end_date FROM academic_year WHERE year_id = ?");
    dateQuery.addBindValue(yearId);

    if(dateQuery.exec() && dateQuery.next())
    {
        QDate end = dateQuery.value("end_date").toDate();
        if(QDate::currentDate() < end)
        {
            qCritical() <<"\033[31m Cannot close academic year: End date" << end.toString("yyyy-MM-dd") <<" has not been reached.";
            Logger::instance().error("Cannot close academic year: End date" + end.toString("yyyy-MM-dd") + " has not been reached.");
            db.rollback();
            return false;
        }

    }


        QSqlQuery updateQuery(db);
        updateQuery.prepare(R"(
                        UPDATE academic_year
                        SET is_active = false
                        WHERE year_id = ?
)");
        updateQuery.addBindValue(yearId);

        if(!updateQuery.exec())
        {
            qCritical() << "\033[31m Failed to close academic year ID: \033[0m" <<yearId <<" " + checkQuery.lastError().text();
            Logger::instance().error("Failed to close academic year ID: " + QString::number(yearId) + " " + checkQuery.lastError().text());
            db.rollback();
            return false;
        }

        QSqlQuery termQuery(db);
        termQuery.prepare("UPDATE terms SET is_active = false WHERE year_id = ?");
        termQuery.addBindValue(yearId);

        if(!termQuery.exec())
        {
            qCritical() <<"\033[31m Failed to close terms for year ID: " <<yearId <<termQuery.lastError().text();
            Logger::instance().error("Failed to close terms for year ID: " + QString::number(yearId) + " " + termQuery.lastError().text());
            db.rollback();
            return false;
        }

        if(!db.commit())
        {
            qCritical() <<"\033[31m Failed to commit transaction. \033[0m" << db.lastError().text();
            Logger::instance().error("Failed to commit transaction. " +db.lastError().text());
            return false;
        }

        qInfo() <<"\033[32m Successfully closed academic year ID: \033[0m" << yearId;
        return true;
    }
    catch(const std::exception& e)
    {
        qCritical() << "\033[31m Exception occurred while closing academic year ID: " << yearId <<  " " << QString(e.what());
        Logger::instance().error("Exception occurred while closing academic year ID: " + QString::number(yearId) + QString(e.what()));
        db.rollback();
        return false;
    }
}





















































