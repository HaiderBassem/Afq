#ifndef ACADEMICYEARDATAHANDLER_H
#define ACADEMICYEARDATAHANDLER_H

#include "DataModel.h"

#include <optional>

#include <QDate>
#include <QString>
#include <QVector>

namespace DataAccess {

class AcademicYearDataHandler {
public:
  static bool addAcademicYear(const DataModel::AcademicYear &year);
  static std::optional<DataModel::AcademicYear> getAcademicYearById(int yearId);
  static QVector<DataModel::AcademicYear> getAllAcademicYears();
  static bool updateAcademicYear(const DataModel::AcademicYear &year);
  static bool deleteAcademicYear(int yearId);

  static std::optional<DataModel::AcademicYear> getCurrentAcademicYear();
  static bool setAcademicYearActive(int yearId);
  static bool isAcademicYearActive(int yearId);
  static QVector<DataModel::AcademicYear> getAcademicYearByDateRange(const QDate &start, const QDate &end);

  static std::optional<int> getAcademicYearCount();
  static std::optional<int> getActiveAcademicYearId();

  static bool isAcademicYearExist(int yearId);
  static bool isAcademicYearNameExist(const QString &name);
  static bool deactivateAllAcademicYears();

  static int getHolidaysCount(int yearId);
  static bool updateHolidaysCount(int yearId, int holidaysCount);
  static int getRemainingHolidays(int yearId);

  static bool isDateInAcademicYear(int yearId, const QDate &date);
  static int getDaysElapsedInAcademicYear(int yearId);
  static int getDaysRemainingInAcademicYear(int yearId);
  static double getAcademicYearProgress(int yearId);
  static int getAcademicYearDuration(int yearId);

  static bool addHolidayToAcademicYear(int yearId, const DataModel::Holiday &holiday);
  static QVector<DataModel::Holiday> getHolidaysByAcademicYear(int yearId);

  static bool closeAcademicYear(int yearId);
};

} // namespace DataAccess
#endif // ACADEMICYEARDATAHANDLER_H
