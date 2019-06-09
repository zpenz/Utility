#include "Contain.h"
#include "Iterator.hpp"
#include <vector>

int main(int argc, char const *argv[])
{
    using namespace Contain;
    using namespace Iterator;

    Linker<int> list;
    list.Add(10);
    list.Add(13);

    list.Delete(10);

    SHOW_MESSAGE(list.size,1);
    return 0;
}
