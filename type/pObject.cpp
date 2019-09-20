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

void Factory::RegisteFunc(const char * name,createfunc func)
{
    utility::show_message(name);
    if(fmap.find(name)!=fmap.end()) return;
    fmap.emplace(pair<const char *,createfunc>(name,func));
}
