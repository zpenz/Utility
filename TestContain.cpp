#include "Contain.h"
#include "Iterator.hpp"
#include "pString.hpp"

//end
template<typename T>
void Test(T arg){
    std::cout<<arg;
}

template<typename T,typename ... type>
void Test(T arg,type ... params)
{
    std::cout<<arg;
    Test(params ...);
}

int main(int argc, char const *argv[])
{
    using namespace Contain;
    using namespace Iterator;

    Linker<AString> list;
    list.Add("cd");
    list.Add("cc");

    list.Delete("ccd");

    SHOW_MESSAGE(*list[1].pData, 1);
    SHOW_MESSAGE(list.size, 1);

    Test(1,"hello world!","zz you","xixi");

    return 0;
}
