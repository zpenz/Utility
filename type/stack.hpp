#pragma once
#include "pTypes.hpp"

template <typename Type>
struct Node{
    Node(Type value):data(std::move(value)){}
    ~Node(){
        // plog("node destruct");
    }
    using RealType = Node<Type>;
    Type data;
    shared_ptr<RealType> child;
    RealType* parent;
};

template <typename Type>
class pStack{
    using StackNodeType = Node<Type>;
    shared_ptr<StackNodeType> CurrentNode;
    unsigned int iSize;
public:
    pStack(){
        iSize = 0;
        CurrentNode = nullptr;
    }
    ~pStack(){
        // plog("stack destruct");
    }
    const Type Pop(){
        shared_ptr<StackNodeType> TempNode(CurrentNode);
        if(iSize>1){
            CurrentNode = shared_ptr<StackNodeType>(CurrentNode->child);
        }
        iSize--;
        return TempNode->data;
    }
    const Type Push(Type data){
        auto makedata = shared_ptr<StackNodeType>(new StackNodeType(data));
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

