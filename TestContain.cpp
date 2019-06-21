#include "Contain.h"
#include "Iterator.hpp"
#include "pString.hpp"
#include "json.hpp"
#include "HttpUtility.hpp"

int main(int argc, char const *argv[])
{
    using namespace Contain;
    using namespace Iterator;
    using namespace Utility;

    // Linker<AString> list;
    // list.Add("cd");
    // list.Add("cc1");

    // SHOW_MESSAGE(list[0], 1);
    // SHOW_MESSAGE(list[1], 1);
    // list[1] = "ooo";
    // SHOW_MESSAGE(list[1],1);
    // SHOW_MESSAGE(list.size, 1);

    // auto a = Utility::is_string<pobject<AString> >::value;

    // Linker<KeyValue> array;
    // array.Add(KeyValue("array",1));
    // array.Add(KeyValue("array1","ooo"));
    // Linker<KeyValue> array2;
    // array2.Add(KeyValue("array2","test"));
    // array2.Add(KeyValue("array3","test2"));
    // array2.Add(KeyValue("array4",array));
    // array2.Add(KeyValue("array5",array));

    // Linker<Linker<KeyValue> > array3;

    Linker<KeyValue> a4(KeyValue("iama","solizia"),KeyValue("xixi","ppp"));

    JObject obj;
    obj.Add("where",1,"iama","solizia","array",a4,"array2","aa","error_code",100,"error_code2",101);
    auto ret = obj.Serial();
    show_message(ret.c_str()); 

    Linker<AString> params;
    params.Add("file");
    params.Add("json.hpp");

    JObject job;
    job.Add("admin_id",1001,"sid","Hd3M1a1jII4MZeUx5fIKswfgWoFVH9l","mount_point","xkraid1560365017","begin_line",1);
    // auto ret2 = Post("http://192.168.10.105:5555/cgi-bin/xk_file_share_directory_get_list_by_mount_point_and_uid.cgi",job.Serial());

    auto ret2 = FormPost("http://192.168.10.105:5555/cgi-bin/xk_file_upload_cover.cgi",params,99999,TransListener(),[](Request& req){
        req.OtherRecord.Add(KV("XK_JSON","{\"admin_id\":1001,\"sid\":\"Hd3M1a1jII4MZeUx5fIKswfgWoFVH9l\",\"path_name\":\"%2Fvar%2Fshare%2Fmp%2Fxkraid1560365017%2Fysc%E6%9D%83%E9%99%90%2F\",\"file_name\":\"ppppppppppssssssss.txt\"}"));
    });
    show_message(ret2); 
    return 0;
}
