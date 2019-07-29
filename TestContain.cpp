#include "Contain.h"
#include "Iterator.hpp"
#include "pString.hpp"
#include "json.hpp"
#include "HttpUtility.hpp"
#include "adler32.hpp"
#include <algorithm>

using namespace Contain;
using namespace Iterator;

int main(int argc, char const *argv[])
{
    // problem
    {
        Linker<int> lk;
        lk.Add(10);
        lk.Add(20);
        plog("..........");
    }

    Utility::JObject obj;
    // AString a = 1;
    obj.Add("where",1);
    // auto ret = obj.Serial();
    // plog(ret); 

    // Linker<AString> params;
    // params.Add("file");
    // params.Add("md5.h");

    Utility::JObject job;
    job.Add("admin_id",1001,"sid","Hd3M1a1jII4MZeUx5fIKswfgWoFVH9l","mount_point","xkraid1560365017","begin_line",1);
    plog(job.Serial());

        // auto ret = LoadDiff<diff>("verify27262990");
        // SaveFile<diff>("diff",CalcFileSlideDiff("test.test"));
        // for_each(ret.begin(),ret.end(),[](diff& item){
        //     plog("index: ",item.index," rvale: ",item.rvalue," md5: ",item.MD5Value);
        // });
        
        // vector<range> oplist = performMarge("test.test","verify27263090");

        // auto oplist = LoadRange("");
        // for_each(oplist.begin(),oplist.end(),[](range& rg){
        //     plog(rg.index," ",rg.length," ",rg.sameblock," ",rg.offset);
        // });

        //----
        // Linker<AString> params;
        // Utility::JObject obj;
        // // obj.Add("admin_id",1001,"sid","npJ8cS1q25M9vQcXXW02H9Te2G2L7O7","directory_path","/var/share/mp/xklvm1562233594/wxy/xk","all_file_name","/var/share/mp/xklvm1562233594/wxy/xk/UtilityTest.cpp");
        // // params.Add("json",obj.Serial());
        // params.Add("file","test.test.op");

        // // auto url = "http://192.168.10.23:5555/cgi-bin/xk_file_rsync_upload.cgi";
        // auto url = "192.168.10.250:9000";
        // FormPostTest(url,params,99999,Utility::TransListener(nullptr,[&](AString string){
        //     plog("result: ",string._length()," content:",string);
            
        // },nullptr,
        // nullptr
        // ),[](Utility::Request& req){
        //             Utility::JObject job;
        //     job.Add("admin_id",1001,"sid","ydanp32o8VTum13m1zoclVu9bbyK6p0","path_name","/var/share/mp/xklvm1562233594/wxy/xk/","file_name","UnitTest.cpp");
        //     req.OtherRecord.Add(Utility::KV("XK_JSON",job.Serial()));
        // });


    return 0;
}
