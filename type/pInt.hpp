#include "pObject.hpp"

class pint : public pTypeObject<int>,virtual public pObject{
    public:

        template<typename T>
        pint(T d):pObject(ObjectType::TYPE_INT),pTypeObject(d){}

        pint operator+=(pint i){data+=i.data;return *this;}
        pint operator-=(pint i){data-=i.data;return *this;}
        pint operator*=(pint i){data*=i.data;return *this;}
        pint operator/=(pint i){data/=i.data;return *this;}
        operator int() {return data;}
};