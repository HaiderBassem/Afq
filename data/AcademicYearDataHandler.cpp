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



        if(year.is_active)
        {
            QSqlQuery deactivateQuery(db);
            deactivateQuery.prepare("UPDATE academic_year SET is_active = false WHERE year_id != ?");
            deactivateQuery.addBindValue(year.year_id);
            deactivateQuery.exec(); // go on even it stop

        }

        if(!db.commit())
        {
            qCritical() <<"\033[31m Failed to commit transaction: \033[0m" + db.lastError().text();
            Logger::instance().error("Failed to commit transaction: " + db.lastError().text());
            db.rollback();
            return false;
        }

        qDebug() <<"\033[32m Updated academic year ID: \033[0m" << QString::number(year.year_id);
        return true;
}
















































