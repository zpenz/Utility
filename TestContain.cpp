#include "Contain.h"
#include "Iterator.hpp"
#include "pString.hpp"
#include <vector>

int main(int argc, char const *argv[])
{
    using namespace Contain;
    using namespace Iterator;

    Linker<AString> list;
    list.Add("cd");
    list.Add("cc");

    list.Delete("ccd");

    SHOW_MESSAGE(*list[1].pData,1);
    SHOW_MESSAGE(list.size,1);
    return 0;
}
