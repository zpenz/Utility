#include "pObject.hpp"
using namespace utility;

shared_ptr<pObject> Factory::create(const char * classname)
{
    auto ret = fmap.find(classname);
    plog(fmap.size());
    for(auto it=fmap.cbegin();it!=fmap.end();it++){
        plog(it->first);
    }

    if(fmap.end()==ret){
        //error
        plog("create error");
        plog(classname);
        return nullptr;
    }
    return ret->second();
}


