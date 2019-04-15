#include <memory>
#include <iostream>
using namespace std;
// template<typename... T>
// void f(T... a){
//     cout<<sizeof...(a)<<endl;
// }

shared_ptr<char> get(){
    shared_ptr<char> ptr = shared_ptr<char>(new char[100],default_delete<char[]>());
}

int main(int argc, char const *argv[])
{
    /* code */
    f(1,2,3,1.0,"");
    return 0;
}
