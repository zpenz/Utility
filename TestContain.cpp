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

    list.Delete("ccd");

    SHOW_MESSAGE(list[1], 1);
    SHOW_MESSAGE(list.size, 1);

    show_message(1,"hello world!","zz you","xixi");

    // auto a = Utility::is_string<pobject<AString> >::value;

    JObject<> obj;
    obj.Add<int>(KeyValue<int>("",1));
    auto a = obj.Serial();
    // show_message(a); 
    return 0;
}
