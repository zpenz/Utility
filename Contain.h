#include "Utility.h"

namespace Contain{
    using namespace utility;
    using namespace smartpoint;
    using namespace stype;
    
    template<typename T>
    struct LinkNode{
        typedef LinkNode<T> Node;
        Node* parent;
        Node* child;
        unique_ptr<T> pData;
        
        inline 
        LinkNode(T data){pData = make_unique<T>(data);}
        inline
        LinkNode(){ 
            pData  = nullptr;
            parent = nullptr;
            child  = nullptr;
            }

        LinkNode(LinkNode<T> && NodeMove){
            parent = NodeMove.parent;
            child  = move(NodeMove.child);
            pData  = move(NodeMove.pData);
        }

        LinkNode(LinkNode<T> & NodeMove){
            parent = NodeMove.parent;
            child  = NodeMove.child;
            pData  = move(NodeMove.pData);
        }
        
        LinkNode<T>& operator=(LinkNode<T>&& NodeMove) {
            parent = NodeMove.parent;
            child  = move(NodeMove.child);
            pData  = move(NodeMove.pData);
            return *this;
        }

        LinkNode<T>& operator=(const LinkNode<T>& NodeMove){
            return *this=move(NodeMove);
        }

        ~LinkNode(){
            // SHOW_MESSAGE("destructor..",1);
            // if(pData) {SHOW_MESSAGE(*pData,1);}
        }
        
        bool operator==(const Node& node){
            if(pData == node.pData)
                return true;
            return false;
        }
        bool operator!=(const Node& node){
            return !(node==*this);
        }

        
    };

    template<typename T>
    struct Linker{
        using Nodetype = LinkNode<T>;
        mutable uint size; 
        Nodetype  head;
        Nodetype* last;
        bool Add(Nodetype node) {
            if(last && last->child)
            last->child = &node;
            if(node.parent)
            node.parent = last;
            last = make_shared<Nodetype>(node).get();
            size++;
            return true;
        }

        bool Delete(Nodetype& node){
            Nodetype* temp = &head;
            while(1){
                temp = temp->child;
                if(node == *temp){
                    if(node.child!=nullptr && node.child->parent!=nullptr)
                        (node.child)->parent = node.parent;
                    if(node.parent!=nullptr && node.parent->child!=nullptr)
                        (node.parent)->child = node.child;
                    node.~LinkNode();
                    size--;
                    return true;
                }
                if(temp==last) break;
                temp = temp->child;
            }
            return false;
        }

        inline
        Linker(Nodetype& node){
            size = 0;
            head = node;
            last = &head;
        }

        inline
        Linker(){
            size = 0;
            last = &head;
        }

        ~Linker() = default;
    };


}
