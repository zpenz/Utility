#pragma once
#include "../Utility.h"
#include <map>
#include <memory>
using namespace std;

enum class ObjectType {
  TYPE_INT,
  TYPE_LONG,
  TYPE_FLOAT,
  TYPE_DOUBLE,
  TYPE_STRING
};

template <typename type> class pTypeObject;

class pObject {
public:
  ObjectType _type;
  pObject(ObjectType tp) { _type = tp; }
  pObject() {}
  virtual ~pObject() {}
};

using createfunc = shared_ptr<pObject> (*)();

class Factory : public pObject {
  SINGLE_INSTANCE(Factory)
  map<const char *, createfunc> fmap;
  shared_ptr<pObject> create(const char *classname);

public:
  template <typename T> 
  void RegisteFunc(createfunc func) {
    const char *name = typeid(T).name();
    utility::show_message(name);
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

template <typename ReflectType> class Reflect : public pObject {
public:
  static auto CreateObject() {
    return dynamic_pointer_cast<pObject>(make_shared<ReflectType>());
  }

  struct reg {
    reg() { FY.RegisteFunc<ReflectType>(CreateObject); }
    inline constexpr void _nothing() {}
  };
  static reg rg;
  Reflect() { rg._nothing(); }
  virtual ~Reflect() {}
};

template <typename T> typename Reflect<T>::reg Reflect<T>::rg;

template <typename Type> class pTypeObject {
public:
  Type data;

  template <typename O> pTypeObject(O o) { data = static_cast<Type>(o); }

  virtual ~pTypeObject() {}
};
