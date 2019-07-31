#pragma once
#include "Utility.h"

namespace Contain{
    using namespace utility;
    using namespace smartpoint;
    using namespace stype;

    template<typename T>
    struct TreeNode{
        typedef TreeNode<T> TreeNodeType;
        shared_ptr<TreeNodeType> lchild;
        shared_ptr<TreeNodeType> rchild;
        shared_ptr<T> data;
        TreeNode(){
            lchild=rchild=data=nullptr;
        }
        TreeNode(const T & Data){
            data = make_shared<T>(move(Data));
        };
    };

    //left=>right
    template<typename T>
    struct Tree{
        typedef TreeNode<T> TreeNodeType;
        shared_ptr<TreeNodeType> base = nullptr;
        Tree(const TreeNodeType head):base(move(head)){}
        Tree(T && headdata){
            base = make_shared<TreeNodeType>(headdata);
        }
        Tree(){}

        void AddLeft(const T & left){ 
            base->lchild = make_shared<TreeNodeType>(move(left));
        }
        void AddRight(const T & right){
            base->rchild = make_shared<TreeNodeType>(move(right));
        }
        void AddLeft(const Tree & lTree){
            base->lchild = move(lTree.base);
        }
        void AddRight(const Tree & rTree){
            base->rchild = move(rTree.base);
        }

        void Add(const T& left,const T & right){
            AddLeft(left);
            AddRight(right);
        }
        void Add(Tree & left,Tree & right){
            AddLeft(left);
            AddRight(right);
        }
        Tree<T>& operator=(const T& data){
            base = make_shared<TreeNodeType>(data);
            return *this;
        }
        Tree<T>& operator=(const Tree& tree){
            base = tree.base;
            return *this;
        }
    };
    
    template<typename T>
    struct LinkNode{
        typedef LinkNode<T> Node;
        shared_ptr<Node> parent;
        shared_ptr<Node> child;
        unique_ptr<T> pData;
        
        LinkNode(const T & data){
            // SHOW_MESSAGE("construct",1);
            pData = make_unique<T>(move(data));
        }
        
        LinkNode(){ 
            pData  = nullptr;
            parent = nullptr;
            child  = nullptr;
        }

        LinkNode(LinkNode<T> && NodeMove){
            parent = move(NodeMove.parent);
            child  = move(NodeMove.child);
            pData  = move(NodeMove.pData);
        }

        LinkNode(LinkNode<T> & node){
            parent = node.parent;
            child  = node.child;
            pData  = make_unique<T>(*node.pData);
        }
        
        LinkNode<T>& operator=(LinkNode<T>&& NodeMove) {
            parent = move(NodeMove.parent);
            child  = move(NodeMove.child);
            pData  = move(NodeMove.pData);
            return *this;
        }

        LinkNode<T>& operator=(const LinkNode<T>& NodeMove){
            parent = NodeMove.parent;
            child  = NodeMove.child;
            pData  = make_unique<T>(*NodeMove.pData);
            return *this;
        }

        ~LinkNode(){
            SHOW_MESSAGE("destructor..",1);
        }
        
        bool operator==(const Node& node) const{
            if(pData == nullptr) return false;
            if(*pData == *node.pData)
                return true;
            return false;
        }
        bool operator!=(const Node& node) const{
            return !(node==*this);
        }

        LinkNode<T>& operator ++(int i){
            CONDITION_MESSAGE(this->child == nullptr,"child nullptr"); 
            if(this->child == nullptr) return *this;
            LinkNode<T> * temp = this->child.get();
            while(i-->=0)
                temp = temp->child.get();
            return *temp;
        }

        LinkNode<T> operator --(){
            return *(this->parent);
        }
        
    };

    template<typename T>
    struct Linker{
        using Nodetype = LinkNode<T>;
        uint size; 
        shared_ptr<Nodetype> head;
        shared_ptr<Nodetype> last;
        Nodetype* cur;

        template<typename U>
        bool Add(U && value) {
            auto temp = make_shared<Nodetype>(move(value));
            if(!head){
                size = 0;
                last = make_shared<Nodetype>();
                head = make_shared<Nodetype>();
                head->child  = last;
                last->parent = head;
                cur = nullptr;
            }
            if(!cur){
                head->child = temp;
                cur = head->child.get();
                head->child->parent = head;
                head->child->child = last;
            }else{
                temp->child  = cur->child;
                cur->child->parent = temp;
                temp->parent = cur->parent->child;
                cur->child   = temp;
                cur = temp.get();
            }
            last->parent = temp;
            size++;
            return true;
        }

        template<typename U,typename ...V>
        bool Add(U u1,V ...v1){
            Add(u1);
            Add(v1...);
            return true;
        }

        bool Delete(T&& value){
            Nodetype node = value;
            if(size==0 ) return false;

            auto temp = head->child;
            while(temp && temp!=last){
                if(node == *temp){
                    temp->child->parent = temp->parent;
                    temp->parent->child = temp->child;

                    node.~LinkNode();
                    size--;
                    return true;
                }
                temp = temp->child;
            }
            return false;
        }

        void Clear(){
            if(size==0 ) return ;
            auto temp = head->child;
            while(temp && temp!=last){
                auto next = temp->child;
                    temp->child->parent = nullptr;
                    temp->child = nullptr;
                    temp->parent->child = nullptr;
                    temp->parent = nullptr;
                    size--;
                temp = next;
            }
            head->child  = last;
            last->parent = head;
            cur = nullptr;
        }

        bool Contain(T value){
            if (size == 0)
                return false;
            Nodetype node = value;
            
            auto temp = head->child;
            while(temp && temp!=last){
                if(node == *temp)
                    return true;
                temp = temp->child;
            }
            return false;
        }
        

        Linker(const T & node){
            Add(node);
        }

        template<class U,class ...V>
        Linker(U node,V...v){
            Add(node);
            Linker(v...);
        }

        Linker(){
            size = 0;
            last = make_shared<Nodetype>();
            head = make_shared<Nodetype>();
            head->child  = last;
            last->parent = head;
            cur = nullptr;
        }

        ~Linker() = default;

        T& operator[](int pos){
            CONDITION_MESSAGE(pos<0 || pos>size-1,"invaild pos");
            LinkNode<T>* temp = head->child.get();
            while(pos--) temp = temp->child.get();
            return *(temp->pData);
        }
    };
}
