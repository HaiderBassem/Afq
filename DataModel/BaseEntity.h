#pragma once


#include<QJsonObject>
#include<QString>
namespace DataModel
{
class BaseEntity
{
public:
    virtual ~BaseEntity() = default;

    // Validation
    virtual bool isValid() const { return true; }

    // JSON serialization
    virtual QJsonObject toJson() const = 0;
    virtual void fromJson(const QJsonObject& json) = 0;

    // Debugging
    virtual QString toString() const = 0;
};
}