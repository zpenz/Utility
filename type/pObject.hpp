#pragma once
enum class ObjectType{
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_DOUBLE
};

class pObject{
    public:
        ObjectType type;
        pObject(ObjectType tp){type = tp;}
    virtual ~pObject(){}
};