#pragma once
#include "Contain.hpp"

using namespace Contain;
template<typename K,typename V>
struct pKeyValue{
    K key;
    V value;
    pKeyValue(K k,V v):key(k),value(v){}
};

template<typename K,typename V>
class pMap{
    typedef Linker<pKeyValue<K,V>> MapType;
    MapType list;

    public:
    void Add(K k,V v){
        list.Add(k,v);
    }

    template<typename ...Ts>
    void Add(pKeyValue<K,V> item,Ts... ts){
        list.Add(item);
        Add(ts...);
    }
    void Add(pKeyValue<K,V> item){
        list.Add(item);
    }

    template<typename ...Ts>
    pMap(Ts... ts){
        Add(ts...);
    }

    V operator[](K k){
        for(int index=0;index<list.size;index++){
            auto temp = list[index];
            if(k==temp.key)
                return temp.value;
        }
        plog("can not find key: ",k);
        return static_cast<V>(0);
    }
    
};