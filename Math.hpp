namespace Math{
    template <typename U>
    U pow(U value,int count){
        for(int index;index<count;index++)
            value*=value;
        return value;
    }    
}