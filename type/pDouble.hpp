#include "pObject.hpp"

class pdouble : public pTypeObject<double, pdouble>,
                public Reflect<pdouble> {
public:
  template <typename T> pdouble(T d) : pTypeObject(d) {}
  pdouble() = default;
  pdouble operator+=(pdouble i) {
    data += i.data;
    return *this;
  }
  pdouble operator-=(pdouble i) {
    data -= i.data;
    return *this;
  }
  pdouble operator*=(pdouble i) {
    data *= i.data;
    return *this;
  }
  pdouble operator/=(pdouble i) {
    data /= i.data;
    return *this;
  }
  operator double() { return data; }
};