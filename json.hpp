#include <type_traits>
#include "pString.hpp"
#include "Contain.h"

namespace Utility
{
    using namespace Contain;

    template<class T>
    struct is_string{
        static char is_str(const char *);
        static char is_str(pobject<const char *>);
        static char is_str(pobject<AString>);
        static double is_str(...);
        static T t;
        enum{ value = sizeof(is_str(t))==sizeof(char)};
    };

    template<typename TValue,typename TKey=AString>
    struct KeyValue:pobject<TValue>{
        TKey key;
        TValue value;
        KeyValue(TKey _key,TValue _value):key(_key),value(_value){}
    };

    template<typename TKey = AString>
    class JObject{
    public:
        Linker<object> list; 

        template<class T>
        void Add(KeyValue<T> && obj){
            list.Add(move(obj));
        }

        template<class T>
        void Set(TKey key,KeyValue<T> obj){
            for(int index=0;index<list.size;index++){
                if(key == list[index]){
                    static_cast<KeyValue<T> >(list[index]).value = obj;
                }
            }
        }

        template<typename TV>
        auto Get(TV key){
            for(int index=0;index<list.size();index++){
                if(key == list[index]){
                    return static_cast<KeyValue<TV> >(list[index]).value;
                }
            }
        }

        TKey Serial(){
            TKey temp = "{";
            for(int index=0;index<list.size;index++){
                // auto item = static_cast<pobject<> >(list[index]);
                // auto pair = static_cast<KeyValue<decltype(item.__type)> >(list[index]);
                auto item = (pobject<double> *)(&list[index]);
                show_message("item_type:",typeid(decltype(item->__type)).name());
                auto pair = (KeyValue<decltype(item->__type)> *)(&list[index]);
                temp+="\"";
                temp+=pair->key;
                temp+="\":";
                if(is_string<decltype(item->__type)>::value){
                    // temp+=AString(static_cast<decltype(item->__type)>(pair->value));
                }else{
                    temp+="\"";
                    // temp+=pair->value;
                    temp+="\"";
                    if(index!=list.size-1)
                    temp+=",";
                }
            }
            temp += "}";
            return temp;
        }
    };

    template<typename TValue,typename TKey = AString>
    class JArray{
        TKey key;
        Linker<JObject<TValue> > list;
    };

    // extern AString SerialJson();
} 
