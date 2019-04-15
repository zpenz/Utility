#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <chrono>

#define SHOW_MESSAGE(message,showline)\
    if(!showline){std::cout<<message<<"\n";}\
    else{std::cout<<"[INFO] "<<message<<" "<<"(in File:"<<__FILE__<<" Func:"<<__FUNCTION__<<" Line:"<<__LINE__<<")\n";}

template <typename Type>
struct Node{
    Node(Type value):data(std::move(value)){}
    ~Node(){SHOW_MESSAGE("node destruct", true);}
    using RealType = Node<Type>;
    Type data;
    std::shared_ptr<RealType> child;
    RealType* parent;
};

template <typename Type>
class StackEx{
    using StackNodeType = Node<Type>;
    std::shared_ptr<StackNodeType> CurrentNode;
    unsigned int iSize;
public:
    StackEx(){
        iSize = 0;
        CurrentNode = nullptr;
    }
    ~StackEx(){
        SHOW_MESSAGE("stack destruct", true);
    }
    const Type Pop(){
        std::shared_ptr<StackNodeType> TempNode(CurrentNode);
        if(iSize>1){
            CurrentNode = std::shared_ptr<StackNodeType>(CurrentNode->child);
        }
        iSize--;
        return TempNode->data;
    }
    const Type Push(Type data){
        auto makedata = std::shared_ptr<StackNodeType>(new StackNodeType(data));
        if(CurrentNode!= nullptr) {
            makedata->child = CurrentNode;
            CurrentNode->parent = makedata.get();
        }
        CurrentNode = makedata;
        iSize++;
        return CurrentNode->data;
    }
    const Type Peek(){
        return CurrentNode->data;
    }
    unsigned int size(){
        return iSize;
    }
};

template <typename Type>
class test
{   
public:
    std::shared_ptr<Type> value;
    void xx(Type* initdata){
        value = std::make_shared<Type>(initdata);
    };
};

template<typename type>
void f(std::vector<type>&& param){
    //do something
}

struct mytype{
    int a;
    char* b;
    mytype()=default;
    mytype(mytype&&){
        std::cout<<"move use"<<std::endl;
    }
    mytype(mytype& type){
        std::cout<<"copy use"<<std::endl;
    }
};

class test2{
    std::string a,b;
    mytype c;
    public:
        test2(){}
        template<typename T>
        void SetValue(T&& c){
            a = std::move(c);
        }
        template<typename T>
        void SetValue2(T&& c){
            this->c = std::move(c);
        }
};

mytype
f(mytype&& ls,const mytype& rhs){
    ls.a+=rhs.a;
    return ls;
    // return std::forward<mytype>(ls);
}

mytype
f2(){
    mytype t1;
    return std::move(t1);
}

mytype
f3(mytype type)
{
    return type;
}

template<typename T>
struct is_pointer{
    template<typename U>
    static char test(U*);
    template<typename U,typename V>
    static char test(V U::*);
    static double test(...);
    static T t;
    static const bool value = sizeof(test(t))==sizeof(char);
};

int main(int argc, char const *argv[])
{
    auto result = is_pointer<int>::value;
    mytype t1,t2;
    t1.a = 10;
    t2.a = 20;
    // auto m3 = f(std::move(t1),t2);
    auto m4 = f2();
    auto time = std::chrono::system_clock::now();
    
    localtime_r
    // auto m5 = f3(t1);
    // Stack<int> stack;
    // stack.Push(1);

    // test<int> test;
    // test.xx(new int(123));
    // static_assert(stack.size()>1,"");
    // std::vector<int> a;
    // f(a);
    // test2 t2;
    // std::string a = "asdfas";
    // mytype b ;
    // b.b = "xxx";
    
    // std::cout<<"before: a="<<a<<std::endl;
    // std::cout<<"before: b="<<b.b<<std::endl;
    // t2.SetValue(a);
    // t2.SetValue2(b);
    // std::cout<<"after:  a="<<a<<std::endl;    
    // std::cout<<"after:  b="<<b.b<<std::endl;    


    return 0;
}
