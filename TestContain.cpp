#include "Contain.h"
#include "Iterator.hpp"
#include "pString.hpp"
#include "json.hpp"
#include "HttpUtility.hpp"

int main(int argc, char const *argv[])
{
    using namespace Contain;
    using namespace Iterator;

    Linker<int> lk;

    Linker<Utility::KeyValue> a4(Utility::KeyValue("iama","solizia"),Utility::KeyValue("xixi","ppp"));

    Utility::JObject obj;
    obj.Add("where",1,"iama","solizia","array",a4,"array2","aa","error_code",100,"error_code2",101);
    auto ret = obj.Serial();
    plog(ret); 

    Linker<AString> params;
    params.Add("file");
    params.Add("test/1.jpg");

    Utility::JObject job;
    job.Add("admin_id",1001,"sid","Hd3M1a1jII4MZeUx5fIKswfgWoFVH9l","mount_point","xkraid1560365017","begin_line",1);
    plog(job.Serial());

    // auto ret2 = Utility::FormPost("http://192.168.10.250:9000/cgi-bin/xk_file_upload_cover.cgi",params,3,Utility::TransListener([](long cur,long tal){
    // },nullptr,nullptr),[](Utility::Request& req){
    //     req.OtherRecord.Add(Utility::KV("XK_JSON","{\"admin_id\":1001,\"sid\":\"Hd3M1a1jII4MZeUx5fIKswfgWoFVH9l\",\"path_name\":\"%2Fvar%2Fshare%2Fmp%2Fxkraid1560365017%2Fysc%E6%9D%83%E9%99%90%2F\",\"file_name\":\"ppppppppppssssssss.txt\"}"));
    // });

    AString hd = "POST cgi-bin/xk_file_upload_cover.cgi HTTP/1.1\nHost: 192.168.10.250:9000\nContent-Type: multipart/form-data; boundary=--xkboundary\n";
    hd+="Content-Length: 48737\nConnection: close\nOrigin: http://192.168.10.250:9000\n";
    hd+="XK_JSON: {\"admin_id\":1001,\"sid\":\"Hd3M1a1jII4MZeUx5fIKswfgWoFVH9l\",\"path_name\":\"%2Fvar%2Fshare%2Fmp%2Fxkraid1560365017%2Fysc%E6%9D%83%E9%99%90%2F\",\"file_name\":\"ppppppppppssssssss.txt\"}";

    auto header = Utility::Request::Parse(hd);
    // for(int index=0;index<header.OtherRecord.size;index++){
    //     plog("key ",header.OtherRecord[index]._key," value ",header.OtherRecord[index]._value);
    // }
    plog("length",header.Host);
    return 0;
}
