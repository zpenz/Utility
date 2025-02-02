#pragma once
#include "Utility.h"
#include <map>
#include <memory>
#include <string.h>
using namespace std;

enum class ObjectType {
  TYPE_INT,
  TYPE_LONG,
  TYPE_FLOAT,
  TYPE_DOUBLE,
  TYPE_STRING
};

template <typename Type,typename ClassType> class pTypeObject;
typedef shared_ptr<char*> safechar;

class pObject {
public:
  char classname[256] = {0};
  ObjectType _type;
  pObject() { 
    const char *name = typeid(pObject).name();
    memcpy(classname,name,sizeof(char) * strlen(name));
  }
  virtual const char * toString() const{
    return classname;
  }
  virtual ~pObject() {}
};

using createfunc = shared_ptr<pObject> (*)();

class Factory : public pObject {
  SINGLE_INSTANCE(Factory)
  map<const char *, createfunc> fmap;

public:
  shared_ptr<pObject> create(const char *classname);

  template <typename T> void RegisteFunc(createfunc func) {
    const char *name = typeid(T).name();
    using namespace utility;
    // plog(name);
    if (fmap.find(name) != fmap.end())
      return;
    fmap.emplace(pair<const char *, createfunc>(name, func));
  }

  template <typename T> shared_ptr<T> create() {
    shared_ptr<pObject> obj = create(typeid(T).name());
    shared_ptr<T> real = dynamic_pointer_cast<T>(obj);
    return real;
  }
};

#define FY Factory::getInstance()

template <typename ReflectType> class Reflect : virtual public pObject {
public:
  static auto CreateObject() {
    return dynamic_pointer_cast<pObject>(make_shared<ReflectType>());
  }

  auto test(){
    return make_shared<ReflectType>();
  }

  typedef ReflectType __type;
  struct reg {
    reg() { FY.RegisteFunc<ReflectType>(CreateObject); }
    inline constexpr void _nothing() {}
  };
  static reg rg;
  Reflect() { rg._nothing(); }
  virtual ~Reflect() {}
};

template <typename T> typename Reflect<T>::reg Reflect<T>::rg;

template <typename Type,typename ClassType> 
class pTypeObject :virtual public pObject{
public:
  Type data;
  template <typename O> pTypeObject(O o) {
    data = static_cast<Type>(o);
    const char *name = typeid(ClassType).name();
    memcpy(classname,name,sizeof(char) * strlen(name));
  }

  pTypeObject() {
    const char *name = typeid(ClassType).name();
    memcpy(classname,name,sizeof(char) * strlen(name));
  };
  virtual ~pTypeObject() {}
};
