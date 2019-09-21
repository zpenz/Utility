#include "pObject.hpp"

class pint :  public pTypeObject<int, pint>,
              public Reflect<pint> {
public:
  template <typename T> pint(T d) : pTypeObject(d) {}
  pint() = default;
  pint operator+=(pint i) {
    data += i.data;
    return *this;
  }
  pint operator-=(pint i) {
    data -= i.data;
    return *this;
  }
  pint operator*=(pint i) {
    data *= i.data;
    return *this;
  }
  pint operator/=(pint i) {
    data /= i.data;
    return *this;
  }
  operator int() { return data; }
};