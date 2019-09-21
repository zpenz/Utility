#include "pObject.hpp"

class pfloat : public pTypeObject<float, pfloat>,
               virtual public Reflect<pfloat> {
public:
  template <typename T> pfloat(T d) : pTypeObject(d) {}
  pfloat() = default;
  pfloat operator+=(pfloat i) {
    data += i.data;
    return *this;
  }
  pfloat operator-=(pfloat i) {
    data -= i.data;
    return *this;
  }
  pfloat operator*=(pfloat i) {
    data *= i.data;
    return *this;
  }
  pfloat operator/=(pfloat i) {
    data /= i.data;
    return *this;
  }
  operator float() { return data; }
};