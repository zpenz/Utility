#pragma once

#include "pString.hpp"
#include "Contain.h"
#include <functional>

namespace Utility
{
    using namespace Contain;
    using hString = AString;
    using KV = KeyValuePair<hString,hString>;
    extern AString::KeyValuePair<hString> CutUrl(const hString& url);

    struct Request:pobject<Request>{
        hString ContentType="";
        hString ContentLength="";
        hString Host="";
        hString Methon="";
        hString Connection="";
        hString Accept="";
        hString AcceptEncoding="";
        hString AcceptLanguage="";
        hString Cookie="";
        hString Origin="";
        hString Referer="";
        hString UserAgent="";
        hString XRequestedWith="";
        hString HttpVersion="HTTP/1.1";

        int port = 80;
        hString ActionAddress="";
        //Other
        Linker<KV> OtherRecord=Linker<KV>();
        Request(hString url);
        hString ToString();
    };

    extern AString FormPost(const hString& url,Linker<hString> list,long timeout = 99999,std::function<void(Request& req)> OtherSetting = nullptr);  
    extern AString Post(const hString& url,const hString& data,long timeout = 99999,std::function<void(Request& req)> OtherSetting = nullptr);   
} // namespace Utility
