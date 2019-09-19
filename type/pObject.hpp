#pragma once
enum class ObjectType{
    TYPE_INT,
    TYPE_LONG,
    TYPE_FLOAT,
    TYPE_DOUBLE
};

template<typename type>
class pTypeObject;

class pObject{
    public:
        ObjectType type;
        pObject(ObjectType tp){type = tp;}
    virtual ~pObject(){}
};

template<typename Type>
class pTypeObject : pObject{
    public:
        Type data;
        template<typename O>
        pTypeObject(O o){
            data = static_cast<Type>(o);
        }

    template<typename T>    
    static void a(){};

    virtual ~pTypeObject(){}
};
