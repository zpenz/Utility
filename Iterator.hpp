namespace Iterator
{
    template<typename _Iterator,typename _Fun>
    void pfor_each(_Iterator begin,_Iterator end,_Fun func){
        for(;begin!=end;begin++){
            func(*begin);
        }
    }

    template<typename _ItType>
    class pIterator{
        
    public:
        using iterator = _ItType*;
        iterator * it;

        _ItType operator*(){
            return *it;
        }    

        virtual iterator operator++() = 0;
        virtual iterator operator--() = 0;
    };
} // namespace Iterator
