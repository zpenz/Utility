#pragma once
#include "Contain.h"
#include "pString.hpp"
#include "type/pTypes.hpp"

namespace Utility {
using namespace Contain;

using JString = AString;

template <class T> struct is_string {
  static char is_str(const char *);
  static char is_str(pobject<const char *>);
  static char is_str(pobject<AString>);
  static double is_str(...);
  static T t;
  enum { value = sizeof(is_str(t)) == sizeof(char) };
};

struct KeyValue : public Reflect<KeyValue> {
  JString key;
  pObject value;
  Linker<KeyValue> list;
  Linker<Linker<KeyValue>> arraylist;
  bool bstr;
  bool blist;
  bool barraylist;

  template <typename PValue> KeyValue(JString _key, PValue _value) : key(_key) {
    bstr = is_string<PValue>::value;
    blist = false;
    barraylist = false;
  }

  KeyValue()=default;
  KeyValue(JString _key, pObject _value) : key(_key), value(_value) {}

  KeyValue(JString _key, Linker<KeyValue> _value) : key(_key) {
    list = _value;
    blist = true;
    barraylist = false;
  }

//   KeyValue(JString _key, Linker<Linker<KeyValue>> _value) : key(_key) {
//     arraylist = _value;
//     blist = false;
//     barraylist = true;
//   }

  JString SerialArrayList() {
    JString temp = "";
    for (int index = 0; index < arraylist.size; index++) {
      auto item = arraylist[index];
      for (int indexY = 0; indexY < item.size; indexY++) {
        temp += "{";
        auto internalItem = item[indexY];
        temp += internalItem.Serial();
        temp += "}";
        if (indexY != item.size - 1)
          temp += ",";
      }
      if (index != arraylist.size - 1)
        temp += ",";
    }
    return temp;
  }

  JString SerialList() {
    JString temp = "{";
    for (int index = 0; index < list.size; index++) {
      auto item = list[index];
      temp += item.Serial();
      if (index != list.size - 1)
        temp += ",";
    }
    temp += "}";
    return temp;
  }

  JString Serial() {
    JString temp = "\"";
    temp += key;
    temp += "\":";
    if (barraylist) {
      temp += "[";
      temp += SerialArrayList();
      temp += "]";
    } else if (blist) {
      temp += "[";
      temp += SerialList();
      temp += "]";
    } else {
      if (bstr) {
        temp += "\"";
        // temp+=value;
        temp += "\"";
      }
      // else
      // temp+=value;
    }
    return temp;
  }

  JString Serial2() {
    JString temp = "\"";
    temp += key;
    temp += "\":";
    if (is_same_class<JString>(value.classname)) {
        temp += "\"";
        temp += value.toString();
        temp += "\"";
    }else if(is_same_class<KeyValue>(value.classname)){
        auto* pointer = &value;
        temp+=dynamic_cast<KeyValue*>(pointer)->Serial2();
    }else if(is_same_class<Linker<KeyValue>>(value.classname)){
        temp += "[";
        auto pointer = dynamic_cast<Linker<KeyValue>*>(&value);
        for(int index=0;index<pointer->size;index++){
            temp+= (*pointer)[index].Serial2();
            if(index!=pointer->size-1)
                temp+=",";
        }
        temp += "]";
    }
    return temp;
  }
};

class JObject {
public:
  Linker<KeyValue> list;

  void Add(KeyValue obj) { list.Add(move(obj)); }

  template <typename Key, typename Value> void Add(Key key, Value value) {
    list.Add(KeyValue(key, value));
  }

  template <typename T, typename U, typename... Ts>
  void Add(T t1, U u1, Ts... ts) {
    list.Add(KeyValue(t1, u1));
    Add(ts...);
  }

  template <class T> void Set(JString key, KeyValue obj) {
    for (int index = 0; index < list.size; index++) {
      if (key == list[index].key) {
        list[index] = obj;
      }
    }
  }

  template <typename TV> auto Get(TV key) {
    for (int index = 0; index < list.size; index++) {
      if (key == list[index]) {
        return list[index].value;
      }
    }
  }

  JString Serial() {
    JString temp = "{";
    for (int index = 0; index < list.size; index++) {
      auto item = list[index];
      temp += item.Serial();
      if (index != list.size - 1)
        temp += ",";
    }
    temp += "}";
    return temp;
  }

  template <typename type> type operator[](JString key) {}
};

class JArray {
  JString key;
  Linker<JObject> list;
};

} // namespace Utility
