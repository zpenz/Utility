#include "pString.hpp"
#include <stdlib.h>

AString W2A(WString src){
    char buf[256];
    mbstate_t mbs;
    const wchar_t* wsrc = src.c_str();
    wcsrtombs(buf,&wsrc,sizeof(buf),&mbs);
    return buf;
}

AString W2AS(WString src,char * buf,size_t length){
    mbstate_t mbs;
    const wchar_t* wsrc = src.c_str();
    wcsrtombs(buf,&wsrc,sizeof(buf),&mbs);
    return buf;
}

WString A2W(AString src){
    wchar_t buf[256];
    mbstowcs(buf,src.c_str(),sizeof(buf));
    return buf;
}

WString A2WS(AString src,wchar_t* dst,size_t size){
    mbstowcs(dst,src.c_str(),size);
    return dst;
}