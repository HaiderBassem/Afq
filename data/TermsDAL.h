#ifndef TERMSDAL_H
#define TERMSDAL_H
#include<optional>
#include<QString>
#include<QVector>
#include<QDate>

#include"DataModels.h"


namespace DataAccess {

class TermsDAL
{
public:
    //CRUD
    static std::optional<int>   createTerm(int yearId, const QString& name, const QDate& start_date, const QDate& end_date);
    static std::optional<DataModel::Term> getTermById(int termId);
    static bool updateTerm(int termId, const QString& name, const QDate& start_date, const QDate& end_date);
    static bool deleteTerm(int termId);


    static QVector<DataModel::Term> getTermByYear(int yearId);
    static std::optional<DataModel::Term> getCurrentTerm();
    static std::optional<DataModel::Term> getTermByName(int yearId, const QString& name);
    static QVector<DataModel::Term> getAllTerms();


    static bool isTermExist(int id);
    static bool isTermActive(int termId);
   // static bool isTermCurrent(int termId);
    static double getTermProgress(int termId);
    static int getTermDuration(int termId);
    static int getRemainingDays(int termId);


    static bool activeTerm(int termId);
    static bool deactivateTerm(int termId);
    // static bool setCurrentTerm(int termId);

};
}
#endif // TERMSDAL_H
