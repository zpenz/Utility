#include "pObject.hpp"

shared_ptr<pObject> Factory::create(const char * classname)
{
    auto ret = fmap.find(classname);
    if(fmap.end()==ret){
        //error
        utility::show_message("error ");
        return nullptr;
    }
    return ret->second();
}


