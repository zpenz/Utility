#pragma once
#include <memory>
#include <map>
#include "../Utility.h"
using namespace std;

enum class ObjectType{
    TYPE_INT,
    TYPE_LONG,
    TYPE_FLOAT,
    TYPE_DOUBLE,
    TYPE_STRING
};

template<typename type>
class pTypeObject;

class pObject{
    public:
        ObjectType _type;
        pObject(ObjectType tp){_type = tp;}
        pObject(){}
    virtual ~pObject(){}
};

using createfunc = shared_ptr<pObject> (*)();

class Factory:public pObject{
    SINGLE_INSTANCE(Factory);
        map<const char*,createfunc> fmap;
    public:
        shared_ptr<pObject> create(const char * classname);
        void RegisteFunc(const char *,createfunc);

        template<typename T>
        shared_ptr<T> create(const char * classname){
            shared_ptr<pObject> obj = create(classname);
            shared_ptr<T> real = dynamic_pointer_cast<T>(obj);
            return real;
        }
};

#define FY Factory::getInstance()

template<typename ReflectType>
class Reflect :public pObject{
    public:
        static auto CreateObject(){
            return make_shared<ReflectType>();
        }

        struct reg{
            reg(){
                FY.RegisteFunc(typeid(ReflectType).name(),CreateObject);
            }
            inline void _nothing(){}
        };
        reg rg;
        Reflect(){rg._nothing();}
        virtual ~Reflect(){}
};

template<typename Type>
class pTypeObject{
    public:
        Type data;

        template<typename O>
        pTypeObject(O o){
            data = static_cast<Type>(o);
        }

    virtual ~pTypeObject(){}
};

