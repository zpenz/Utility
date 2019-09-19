#include "pObject.hpp"

class pdouble : public pTypeObject<double>,public virtual pObject{
    public:
        template<typename T>
        pdouble(T d):pObject(ObjectType::TYPE_DOUBLE),pTypeObject(static_cast<double>(d)){}

        pdouble operator+=(pdouble i){data+=i.data;return *this;}
        pdouble operator-=(pdouble i){data-=i.data;return *this;}
        pdouble operator*=(pdouble i){data*=i.data;return *this;}
        pdouble operator/=(pdouble i){data/=i.data;return *this;}
        operator double() {return data;}
};