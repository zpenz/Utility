#include <iostream>
#include <future>
#include <chrono>
using namespace std;
using namespace std::chrono_literals;
using namespace std;

class A{
    public:

};

constexpr int operator ""_szp(unsigned long long cx){
    return 10;
}


void f(){

    auto ret = async(launch::deferred,[](){
        for(int i=0;i<10;i++)
        cout<<"run aync!"<<endl;
    });
            ret.wait_for(2000ms);
                 ret.get();
        for(int i=0;i<10;i++)
    cout<<"run after!"<<endl;




}

int main(int argc, char const *argv[])
{
    auto zz = 10'100'100;
    cout<<"run before!"<<endl;
    f();
    auto a = 123_szp;
    cout<<a<<endl;
    return 0;
}
