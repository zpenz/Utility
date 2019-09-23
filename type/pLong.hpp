#include "pObject.hpp"

class plong : public pTypeObject<long, plong>, 
              public Reflect<plong> {
public:
  template <typename T> plong(T d) : pTypeObject(d) {}
  plong() = default;
  plong operator+=(plong i) {
    data += i.data;
    return *this;
  }
  plong operator-=(plong i) {
    data -= i.data;
    return *this;
  }
  plong operator*=(plong i) {
    data *= i.data;
    return *this;
  }
  plong operator/=(plong i) {
    data /= i.data;
    return *this;
  }
  operator long() { return data; }
};