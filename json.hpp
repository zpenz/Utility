#include <type_traits>
#include "pString.hpp"
#include "Contain.h"

namespace Utility
{
    using namespace Contain;

    template<typename TValue,typename TKey=AString>
    struct KeyValue:pobject{
        TKey key;
        TValue value;
        KeyValue(_key,_value):key(_key),value(_value){}
    };

    template<typename TValue,typename TKey = AString>
    class JObject{
    public:
        Linker<pobject> list;

        void Add(pobject obj){
            list.add(obj);
        }

        void Set(TKey key,pobject obj){
            for(int index=0;index<list.size();index++){
                if(key == list[index]){
                    ((KeyValue)list[index]).value = obj;
                }
            }
        }

        auto Get(TKey key){
            for(int index=0;index<list.size();index++){
                if(key == list[index]){
                    return list[index];
                }
            }
        }

        TKey Serial(){
            TKey = "{";
            for(int index=0;index<list.size();index++){
                KeyValue item = (KeyValue)list[index];
                Tkey+="\"";
                Tkey+=item.key;
                Tkey+="\":";
                Tkey+=item.value;
            }
            TKey += "}";
        }
    };

    template<typename TValue,typename TKey = AString>
    class JArray{
        TKey key;
        Linker<JObject<TValue,TKey> > list;
    };

    extern AString SerialJson();
} 
