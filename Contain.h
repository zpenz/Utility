#include "Utility.h"

namespace Contain{
    using namespace utility;
    using namespace smartpoint;
    using namespace stype;
    
    template<typename T>
    struct LinkNode{
        typedef LinkNode<T> Node;
        shared_ptr<Node> parent;
        shared_ptr<Node> child;
        unique_ptr<T> pData;
        
        LinkNode(const T & data){
            SHOW_MESSAGE("construct",1);
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
            SHOW_MESSAGE("move construct",1);
        }

        LinkNode(LinkNode<T> & node){
            parent = node.parent;
            child  = node.child;
            pData  = make_unique<T>(*node.pData);
            SHOW_MESSAGE("copy construct",1);
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
            if(pData) {SHOW_MESSAGE(*pData,1);}
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

        LinkNode<T>& operator ++(){
            CONDITION_MESSAGE(this->child == nullptr,"child nullptr"); 
            if(this->child == nullptr) return *this;
            this = *(this->child);
            return *this;
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

        bool Add(T && value) {
            auto temp = make_shared<Nodetype>(move(value));
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

        inline
        Linker(Nodetype& node){
            Linker();
            Add(node);
        }

        inline
        Linker(){
            size = 0;
            last = make_unique<Nodetype>();
            head = make_unique<Nodetype>();
            head->child  = last;
            last->parent = head;
            cur = nullptr;
        }

        ~Linker() = default;

        LinkNode<T> operator[](int pos){
            CONDITION_MESSAGE(pos<0 || pos>size-1,"invaild pos");
            LinkNode<T> temp = *(head->child);
            while(pos--){
                temp = *(temp.child);
            }
            return temp;
        }
    };


}
