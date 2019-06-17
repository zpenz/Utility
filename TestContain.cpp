#include "Contain.h"
#include "Iterator.hpp"
#include "pString.hpp"
#include "json.hpp"

int main(int argc, char const *argv[])
{
    using namespace Contain;
    using namespace Iterator;
    using namespace Utility;

    Linker<AString> list;
    list.Add("cd");
    list.Add("cc");

    SHOW_MESSAGE(list[0], 1);
    SHOW_MESSAGE(list[1], 1);
    list[1] = "ooo";
    SHOW_MESSAGE(list[1],1);
    SHOW_MESSAGE(list.size, 1);

    auto a = Utility::is_string<pobject<AString> >::value;

    JObject obj;
    Linker<KeyValue> array;
    array.Add(KeyValue("array",1));
    array.Add(KeyValue("array","ooo"));
    Linker<KeyValue> array2;
    array2.Add(KeyValue("array2","test"));
    array2.Add(KeyValue("array2","test2"));
    array2.Add(KeyValue("array1",array));
    array2.Add(KeyValue("array1",array));

    obj.Add(KeyValue("where",1));
    obj.Add(KeyValue("iama","solizia"));
    obj.Add(KeyValue("array",array));
    obj.Add(KeyValue("array2",array2));
    auto ret = obj.Serial();
    show_message(ret.c_str()); 
    return 0;
}
