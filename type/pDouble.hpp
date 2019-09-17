#include "pObject.hpp"

class pDouble : public pObject{
    public:
        double data;
        pDouble(double i):pObject(ObjectType::TYPE_DOUBLE),data(i){}
        pDouble operator=(double i){data=i;return *this;}
        pDouble operator+=(double i){data+=i;return *this;}
        pDouble operator-=(double i){data-=i;return *this;}
        pDouble operator*=(double i){data*=i;return *this;}
        pDouble operator/=(double i){data/=i;return *this;}

};