#include "pObject.hpp"

class pInt : public pObject{
    public:
        int data;
        pInt(int i):pObject(ObjectType::TYPE_INT),data(i){}
        pInt operator=(int i){data=i;return *this;}
        pInt operator+=(int i){data+=i;return *this;}
        pInt operator-=(int i){data-=i;return *this;}
        pInt operator*=(int i){data*=i;return *this;}
        pInt operator/=(int i){data/=i;return *this;}
};