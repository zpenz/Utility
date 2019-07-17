#include "Contain.h"
#include "Iterator.hpp"
#include "pString.hpp"
#include "json.hpp"
#include "HttpUtility.hpp"
#include "adler32.hpp"

using namespace Contain;
using namespace Iterator;

int main(int argc, char const *argv[])
{
    // problem
    // {
    //     Linker<int> lk;
    //     lk.Add(10);
    //     lk.Add(20);
    //     // lk.Clear();
    //     plog("..........");
    // }

    // Linker<Utility::KeyValue> a4(Utility::KeyValue("iama","solizia"),Utility::KeyValue("xixi","ppp"));

    // Utility::JObject obj;
    // obj.Add("where",1,"iama","solizia","array",a4,"array2","aa","error_code",100,"error_code2",101);
    // auto ret = obj.Serial();
    // plog(ret); 

    // Linker<AString> params;
    // params.Add("file");
    // params.Add("md5.h");

    // Utility::JObject job;
    // job.Add("admin_id",1001,"sid","Hd3M1a1jII4MZeUx5fIKswfgWoFVH9l","mount_point","xkraid1560365017","begin_line",1);
    // plog(job.Serial());

    Linker<AString> params;
    Utility::JObject obj;
    obj.Add("admin_id",1001,"sid","iIF1Ss9pCq1Am08sUWz02k8Wq874g8E","directory_path","/var/share/mp/xklvm1562233594/photo/","all_file_name","/var/share/mp/xklvm1562233594/photo/Licence.txt");
    params.Add("json",obj.Serial());

    fstream fs = fstream("1.diff",fs.out);
    FormPost("http://192.168.10.23:5555/cgi-bin/xk_file_rsync_verify.cgi",params,99999,Utility::TransListener(nullptr,[&](AString string){
        fs.close();
        plog("result: ",string._length()," content:",string);
        
        // auto ret = LoadDiff<diff>("1.diff");
        // for_each(ret.begin(),ret.end(),[](diff &item){
        //     plog(item.MD5Value," ",item.rvalue);
        // });
    },nullptr,//nullptr
    [&](AString rece,Utility::Response& spo){
        fs.write(rece.c_str(),rece._length());
        plog(rece);
    }
    ),[](Utility::Request& req){
    });

    return 0;
}
