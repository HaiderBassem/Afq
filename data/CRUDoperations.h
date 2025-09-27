#ifndef CRUDOPERATIONS_H
#define CRUDOPERATIONS_H

namespace Interfaces {

template <typename T>
class CRUDoperations
{
public:
    virtual bool Add(const T& data) =0;
    // virtual static bool Get(const auto& data) = 0;
    // virtual static bool GetAll() = 0;
    // virtual static bool Update(const auto& data) = 0;
    // virtual static bool Delete(const auto& data) = 0;
};
}
#endif // CRUDOPERATIONS_H
