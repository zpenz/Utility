#include "pObject.hpp"

class pFloat : public pObject{
    public:
        float data;
        pFloat(float i):pObject(ObjectType::TYPE_FLOAT),data(i){}
        pFloat operator=(float i){data=i;return *this;}
        pFloat operator+=(float i){data+=i;return *this;}
        pFloat operator-=(float i){data-=i;return *this;}
        pFloat operator*=(float i){data*=i;return *this;}
        pFloat operator/=(float i){data/=i;return *this;}

};