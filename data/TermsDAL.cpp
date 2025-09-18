#include "TermsDAL.h"
#include"DatabaseManager/databaseManager.h"
#include<QSqlQuery>
#include<QSqlDatabase>
#include<QSqlError>
#include<optional>
#include<QDate>


std::optional<int> DataAccess::TermsDAL::createTerm(int yearId, const QString &name, const QDate &start_date, const QDate &end_date)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare(R"(
        INSERT INTO terms(year_id, name, start_date, end_date)
        VALUES (:yearId, :name, :start, :end)
        RETURNING term_id
    )");
    query.bindValue(":yearId", yearId);
    query.bindValue(":name", name);
    query.bindValue(":start", start_date.toString("yyyy-MM-dd"));
    query.bindValue(":end", end_date.toString("yyyy-MM-dd"));

    if (query.exec() && query.next())
        return query.value(0).toInt();

    //TODO: logging
    qWarning() << "CREATE TERM ERROR:" << query.lastError().text();
    return std::nullopt;
}

std::optional<DataModel::Term> DataAccess::TermsDAL::getTermById(int termId)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare("SELECT * FROM terms WHERE term_id = :termId");
    query.bindValue(":termId", termId);

    if (query.exec() && query.next())
    {
        DataModel::Term trm;
        trm.term_id   = query.value("term_id").toInt();
        trm.year_id   = query.value("year_id").toInt();
        trm.name      = query.value("name").toString();
        trm.start_date = query.value("start_date").toDate();
        trm.end_date   = query.value("end_date").toDate();
        return trm;
    }

    // TODO: logging
    qWarning() << "GET TERM BY ID ERROR:" << query.lastError().text();
    return std::nullopt;
}

bool DataAccess::TermsDAL::updateTerm(int termId, const QString &name, const QDate &start_date, const QDate &end_date)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    if (!db.transaction())
    {
        qWarning() << "FAILED TO START TRANSACTION:" << db.lastError().text();
        return false;
    }

    QSqlQuery query(db);
    query.prepare(R"(
        UPDATE terms
        SET name = :name,
            start_date = :start,
            end_date = :end
        WHERE term_id = :termId
    )");

    query.bindValue(":termId", termId);
    query.bindValue(":name", name);
    query.bindValue(":start", start_date.toString("yyyy-MM-dd"));
    query.bindValue(":end", end_date.toString("yyyy-MM-dd"));

    if (!query.exec())
    {
        qWarning() << "UPDATE TERM ERROR:" << query.lastError().text();
        db.rollback();
        return false;
    }

    if (!db.commit())
    {
        qWarning() << "FAILED TO COMMIT TRANSACTION:" << db.lastError().text();
        db.rollback();
        return false;
    }

    return query.numRowsAffected() > 0;
}

bool DataAccess::TermsDAL::deleteTerm(int termId)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare("DELETE FROM terms WHERE term_id = :termId");

    if(!query.exec())
    {
        //TODO: logging
        qWarning()<<"DELETE TERM ERROR:"<< query.lastError().text();
        return false;
    }
    return query.numRowsAffected() > 0;
}

QVector<DataModel::Term> DataAccess::TermsDAL::getTermByYear(int yearId)
{
    QVector<DataModel::Term> terms;
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare("SELECT * FROM terms WHERE year_id = :yearId ORDER BY start_date ASC ");
    query.bindValue(":yearId",yearId);

    if(!query.exec())
    {
        qWarning() << "GET TERMS BY YEAR ERROR:" << query.lastError().text();
        return terms;
    }

    while(query.next())
    {
        DataModel::Term trm;
        trm.term_id = query.value("term_id").toInt();
        trm.year_id = query.value("year_id").toInt();
        trm.name = query.value("name").toString();
        trm.start_date = query.value("start_date").toDate();
        trm.end_date = query.value("end_date").toDate();
        terms.append(trm);
    }
    return terms;

}

std::optional<DataModel::Term> DataAccess::TermsDAL::getCurrentTerm()
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare("SELECT * FROM terms WHERE WHERE CURRENT_DATE BETWEEN start_date AND end_date LIMIT 1 ");
    if(!query.exec())
    {
        qWarning() << "GET CURRENT TERM ERROR:" << query.lastError().text();
        return std::nullopt;
    }

    if(query.next())
    {
        DataModel::Term trm;
        trm.term_id = query.value("term_id").toInt();
        trm.year_id = query.value("year_id").toInt();
        trm.name = query.value("name").toString();
        trm.start_date = query.value("start_date").toDate();
        trm.end_date = query.value("end_date").toDate();
        return trm;
    }
    return std::nullopt;
}

std::optional<DataModel::Term> DataAccess::TermsDAL::getTermByName(int yearId, const QString &name)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare("SELECT * FROM terms WHERE name = :name LIMIT 1 ");
    query.bindValue(":name",name);

    if(!query.exec())
    {
        qWarning()<<"GET TERM BY NAME ERROR:"<<query.lastError().text();
        return std::nullopt;
    }

    if(query.next())
    {
        DataModel::Term trm;
        trm.term_id = query.value("term_id").toInt();
        trm.year_id = query.value("year_id").toInt();
        trm.name = query.value("name").toString();
        trm.start_date = query.value("start_date").toDate();
        trm.end_date = query.value("end_date").toDate();
        return trm;
    }
    return std::nullopt;
}

QVector<DataModel::Term> DataAccess::TermsDAL::getAllTerms()
{
    QVector<DataModel::Term> terms;
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);


    if(!query.exec("SELECT * FROM terms"))
    {
        qWarning() << "GET ALL TERMS ERROR:" << query.lastError().text();
        return terms;
    }

    while(query.next())
    {
        DataModel::Term trm;
        trm.term_id = query.value("term_id").toInt();
        trm.year_id = query.value("year_id").toInt();
        trm.name = query.value("name").toString();
        trm.start_date = query.value("start_date").toDate();
        trm.end_date = query.value("end_date").toDate();
        terms.append(trm);
    }
    return terms;

}

bool DataAccess::TermsDAL::isTermExist(int id)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    QSqlQuery query(db);
    query.prepare("SELECT 1 FROM terms WHERE term_id=:id");
    query.bindValue(":id", id);

    if(query.exec() && query.next())
        return true;

    qWarning() << "IS TERM EXIST ERROR:" << query.lastError().text();
    return false;
}

bool DataAccess::TermsDAL::isTermActive(int termId)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    QSqlQuery query(db);
    query.prepare("SELECT 1 FROM terms WHERE term_id = :id AND is_active = true");
    query.bindValue(":id", termId);

    if (!query.exec()) {
        qWarning() << "IS TERM ACTIVE ERROR:" << query.lastError().text();
        return false;
    }

    return query.next(); // it will return one column if it exist
}

double DataAccess::TermsDAL::getTermProgress(int termId)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    QSqlQuery query(db);
    query.prepare("SELECT start_date, end_date FROM terms WHERE term_id = :id");
    query.bindValue(":id",termId);


    if (!query.exec() || !query.next())
    {
        qWarning() << "GET TERM PROGRESS ERROR:" << query.lastError().text();
        return 0.0;
    }

    QDate start = query.value("start_date").toDate();
    QDate end = query.value("end_date").toDate();
    QDate today = QDate::currentDate();

    if(!start.isValid() || !end.isValid() || start > end)
    {
        qWarning()<< "INVALID TERM DATES FROM TERM ID: "<<termId;
        return 0.0;
    }

    int totalDays = start.daysTo(end) + 1; // with end date
    int elapseDays = start.daysTo(today);

    if(today < start) return 0.0;
    if(today > end) return 100;


    double progress = (static_cast<double> (elapseDays) / totalDays) * 100;
    return progress;
}

int DataAccess::TermsDAL::getTermDuration(int termId)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    QSqlQuery query(db);
    query.prepare("SELECT start_date, end_date FROM terms WHERE term_id = :id");
    query.bindValue(":id", termId);


    if (!query.exec() || !query.next())
    {
        qWarning() << "GET TERM DURATION ERROR:" << query.lastError().text();
        return 0;
    }

    QDate start = query.value("start_date").toDate();
    QDate end = query.value("end_date").toDate();

    if(!start.isValid() || !end.isValid() || start > end) return 0;

    return start.daysTo(end) + 1; // include the end day
}

int DataAccess::TermsDAL::getRemainingDays(int termId)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    QSqlQuery query(db);

    query.prepare("SELECT end_date FROM terms WHERE term_id = :id");

    query.bindValue(":id",termId);

    if (!query.exec() || !query.next())
    {
        qWarning() << "GET REMAINING DAYS ERROR:" << query.lastError().text();
        return 0;
    }

    QDate end = query.value("end_date").toDate();
    QDate today = QDate::currentDate();

    if(!end.isValid())
        return 0;

    if(today > end)
        return 0;

    return today.daysTo(end) + 1;
}

bool DataAccess::TermsDAL::activeTerm(int termId)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare("UPDATE terms SET is_active = true WHERE term_id = :termId");

    query.bindValue(":termId",termId);

    if(!query.exec())
    {
            qWarning() << "ACTIVE TERM ERROR:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

bool DataAccess::TermsDAL::deactivateTerm(int termId)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare("UPDATE terms SET is_active = false WHERE term_id = :termId");

    query.bindValue(":termId",termId);

    if(!query.exec())
    {
        qWarning() << "DEACTIVE TERM ERROR:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}



// bool DataAccess::TermsDAL::setCurrentTerm(int termId)
// {

// }








