#pragma once
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

    #define SAFE_CLOSE(subject)\
        if(subject){ fclose(subject);}

    #define plog(params...)\
        {show_message(params);\
        cout<<"                          "<<"(in File:"<<__FILE__<<" Func:"<<__FUNCTION__<<" Line:"<<__LINE__<<")\n";}

    #define CONDITION_MESSAGE(condition,message...)\
    if(condition){ plog(message);}

    template<typename T>
    void show_message(T param){
        cout<<param;
    }

    template<typename T,typename ...Params>
    void show_message(T param,Params... args){
        cout<<param;
        show_message(args ...);
    }
    
    struct object{
        virtual ~object(){};
    };

    template<typename T = object>
    struct pobject:object{
        static T __type;
        virtual ~pobject(){};
    };

    template<typename T,typename Z>
    struct KeyValuePair{
        T _key;
        Z _value;

        template<class U,class P>
        KeyValuePair(U key,P value):_key(key),_value(value){}
    };

    #define SINGLE_INSTANCE(classname) \
		private: \
			classname(){} \
		public:\
		inline static classname & getInstance(){\
			static classname  cInstance;\
			return cInstance;}
    
    template<typename T>
    T ppow(T value, int count){
      T sum = 1;
      for (int index = 0; index < count; index++)
        sum *= value;
      return sum;
    };
}
