#include "Contain.h"

int main(int argc, char const *argv[])
{
    using namespace Contain;

    Linker<int> list;
    list.Add(move(10));
    list.Add(13);

    LinkNode<int> temp = 10;
    list.Delete(temp);

    SHOW_MESSAGE(list.size,1);
    return 0;
}
