
#include <iostream>
#include <memory>

namespace smartpoint{
    
}

namespace stype{
    using uint = unsigned int;
    using ulong = unsigned long;
}

namespace utility{
    using namespace std;

    #define SHOW_MESSAGE(message,showline)\
        if(!showline){cout<<message<<"\n";}\
        else{cout<<"[INFO] "<<message<<"                          "<<"(in File:"<<__FILE__<<" Func:"<<__FUNCTION__<<" Line:"<<__LINE__<<")\n";}

    #define CONDITION_MESSAGE(condition,message)\
        if(condition){ SHOW_MESSAGE(message,true);}

    #define SAFE_CLOSE(subject)\
        if(subject){ fclose(subject);}
 
}
