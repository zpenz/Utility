#include "pObject.hpp"

class pfloat : public pTypeObject<float>,public virtual pObject{
    public:

        template<typename T>
        pfloat(T d):pObject(ObjectType::TYPE_FLOAT),pTypeObject(d){}

        pfloat operator+=(pfloat i){data+=i.data;return *this;}
        pfloat operator-=(pfloat i){data-=i.data;return *this;}
        pfloat operator*=(pfloat i){data*=i.data;return *this;}
        pfloat operator/=(pfloat i){data/=i.data;return *this;}
        operator float() {return data;}
};