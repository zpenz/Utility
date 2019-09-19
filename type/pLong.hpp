#include "pObject.hpp"

class plong : public pTypeObject<long>,virtual public pObject{
    public:

        template<typename T>
        plong(T d):pObject(ObjectType::TYPE_LONG),pTypeObject(d){}

        plong operator+=(plong i){data+=i.data;return *this;}
        plong operator-=(plong i){data-=i.data;return *this;}
        plong operator*=(plong i){data*=i.data;return *this;}
        plong operator/=(plong i){data/=i.data;return *this;}
        operator long() {return data;}
};