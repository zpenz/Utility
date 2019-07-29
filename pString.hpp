 #pragma once
 #include <memory>
 #include <iostream>
 #include <type_traits>
 #include <string>
 #include <fstream>
 #include <cctype>
 #include <cmath>
 #include <string.h>
 #include "Contain.h"

 #include "Utility.h"
 using namespace utility;

 template<typename T>
    class String{
        shared_ptr<T> buffer;
        int length;
        int size;
  public:
        static bool IsIntC(const T c){
            return (c>='0' && c<='9') || (c>='a' && c<='f') || (c>='A' && c<='F');
        }

        static bool IsInt(const String<T>& des){
            int pos = 0;
            while(pos<des.length)
                if(!IsIntC(des.buffer.get()[pos++])) return false;
            return true;
        }

        static int ToIC(const T c){
            CONDITION_MESSAGE(!IsIntC(c),"invalid conver int");
            if(c=='a' || c=='A') return 10;
            if(c=='b' || c=='B') return 11;
            if(c=='c' || c=='C') return 12;
            if(c=='d' || c=='D') return 13;
            if(c=='e' || c=='E') return 14;
            if(c=='f' || c=='F') return 15;
            return c-'0';
        }

        static long long ToL(const String<T>& des){
            long long sum = 0;
            bool negative = des.buffer.get()[0]=='-';
            for(int index=negative?1:0;index<des.length;index++){
                if(des.buffer.get()[index] == ' ') continue; //skip empty
                sum = 10*sum+ToIC(des.buffer.get()[index]);
            }
            return negative?-1*sum:sum;
        }

        static long long ToLH(const String<T>& des){
            long long sum = 0;
            bool negative = des.buffer.get()[0]=='-';
            for(int index=negative?1:0;index<des.length;index++){
                if(des.buffer.get()[index] == ' ') continue; //skip empty
                sum = 16*sum+ToIC(des.buffer.get()[index]);
            }
            return negative?-1*sum:sum;
        }

        ///1.calc length 2.needvalue=pow(10,cout)%10 3.put into tempbuffer
        static const String<T> LToS(long long value){
            auto pow = [](int value,int count)->long long{
                long long sum = 1;
                for(int index =0;index<count;index++)
                    sum*=value;
                return sum;
            };
            bool negative = value<0;
            if(negative) value = -1*value;
            int length = 1;
            long long temp = value;
            while(temp/=10) length++;
            T tempbuffer[length+1+(negative?1:0)];
            int index = length-1;
            while(index>=0){
                tempbuffer[length-(negative?0:1)-index] = '0' + value/pow(10,index)%10;
                index--;
            }
            if(negative) tempbuffer[0]='-';
            tempbuffer[length+(negative?1:0)] = 0;
            return tempbuffer;
        }

        static const String<T> DToS(double value,int effective){
            long long intpart = static_cast<long long>(value);
            double diff = value-intpart;
            if(diff==0.0f) return LToS(static_cast<long long>(value));
            double smallest =pow(0.1f,effective);
            // smallest = 0.1f*0.1f*0.1f;
            while(diff-static_cast<long long>(diff)>smallest) 
                diff*=10;
            long long nextc = (static_cast<long long>(diff*10))%10;
            //calc . right value
            SHOW_MESSAGE(smallest,1);
            SHOW_MESSAGE(diff,1);
            SHOW_MESSAGE(nextc,1);
            diff = (long long)diff+(nextc>4?1:0);
            return String<T>(intpart)+ String<T>(".")+ String<T>(diff);
        }

        static const String<T> DToS(double value){
            long long intpart = static_cast<long long>(value);
            double diff = value-intpart;
            if(diff==0.0f) return LToS(static_cast<long long>(value));
            while(diff-static_cast<long long>(diff)!=0.0f) 
                diff*=10;
            return String<T>(intpart)+ String<T>('.')+ String<T>(diff);
        }

        static bool Empty(const String<T>& des){
            return des.length == 0;
        }
        static bool Compare(const String<T>& src,const String<T>& des)
        {
            auto length = src.length>=des.length?src.length:des.length;
            auto SrcPointer = src.buffer.get();
            auto DesPointer = des.buffer.get();
            for(int index=0;index<length;index++){
                int internal = *SrcPointer++-*DesPointer++;
                if(internal!=0) return internal?1:-1;
                if(!*SrcPointer || !*DesPointer) return src.length-des.length;
            }
            return 0;
        }
        static int CalcLength(const T* src){
            CONDITION_MESSAGE(src==nullptr,"null pointer");
            int size = 0;
            while(*src++) size++;
            return size;
        }
        
        String(){
            buffer = nullptr;
            length = 0;
            size = 0;
        }
        
        String(const T a){
            buffer = shared_ptr<T>(new T[2],default_delete<T[]>());
            buffer.get()[0] = a;
            length = 1;
            size = sizeof(T)*2;
            buffer.get()[length]=0;
        }

        String(long value){
            *this = LToS(value);
        }

        String(int value){
            *this = LToS(value);
        }

        String(long long value){
            *this = LToS(value);
        }

        String(double value){
            *this = DToS(value);
        }

        String(double value,int effective){
            *this = DToS(value,effective);
        }

        String(const T* desBuffer){
            CONDITION_MESSAGE(nullptr==desBuffer,"invalid buffer param!");
            length = CalcLength(desBuffer);
            buffer = shared_ptr<T>(new T[length+1],default_delete<T[]>());
            size = sizeof(T)*length;
            memcpy(buffer.get(),desBuffer,size);
            (buffer.get())[length] = 0;
        }

        String(const String<T>&src,int start,int end){
            CONDITION_MESSAGE(start>src.length || start<0 || end<0 || end>src.length || end<start,"invalid length param!");
            length = end-start;
            buffer = shared_ptr<T>(new T[length+1],default_delete<T[]>());
            size = sizeof(T)*length;
            memcpy(buffer.get(), src.buffer.get()+start, size);
            (buffer.get())[length] = 0;
        }
        
        String(const String<T>& src,int ilength) { 
            CONDITION_MESSAGE(ilength>src.length || ilength<0,"invalid length param!");
            length = ilength;
            buffer = shared_ptr<T>(new T[length+1],default_delete<T[]>());
            size = sizeof(T)*length;
            memcpy(buffer.get(), src.buffer.get(), size);
            (buffer.get())[length] = 0;
        }

        //COPY
        String(const String<T>& src){
            length = src.length;
            size = src.size;
            buffer = shared_ptr<T>(new T[length+1],default_delete<T[]>());
            memcpy(buffer.get(),src.buffer.get(),(length)*sizeof(T));
            (buffer.get())[length] = 0;
        }

        //MOVE
        String(const String<T>&& src){
            length = src.length;
            size = src.size;
            buffer = move(src.buffer);
        }

        ~String(){
            // SHOW_MESSAGE("destructor...",1); 
        }
        
        unsigned int _size() const{
            return size;
        }
        unsigned int _length() const{
            return length;
        }
        operator const T*(){
            return c_str();
        }

        const T operator[](int index){
            CONDITION_MESSAGE(index<0 || index>length-1,"invalid index:");
            return buffer.get()[index];
        }

        const T* c_str() const{
            return buffer.get();
        }

        const T At(int pos) {
            CONDITION_MESSAGE(pos>length || pos<0,"invalid pos param!");
            return buffer.get()[pos];
        }

        int IndexOf(const T element){
            for(int index=0;index<length;index++)
                if((buffer.get())[index] == element) return index;
            return -1;
        }

        int rIndexOf(const T element){
            for(int index=length-1;index>=0;index--)
                if((buffer.get())[index] == element) return index;
            return -1;
        }

        int find(const String<T>& src) const{
            if(src.length>length) return -1;
            int pos = 0;
            while(pos<length){
                if((buffer.get())[pos] == (src.buffer.get())[0]){
                    if(pos+src.length>length) break;
                    for(int index=0;index<src.length;index++)
                        if((buffer.get())[pos+index] != (src.buffer.get())[index]) goto goon;
                    return pos;
                }
            goon:
                pos++;
            }
            return -1;
        }

        int rfind(const String<T>& src) const{
            if(src.length>length) return -1;
            int pos = length-1;
            while(pos>=0){
                if((buffer.get())[pos] == (src.buffer.get())[0]){
                    if(pos+src.length>length) goto goon;
                    for(int index=0;index<src.length;index++)
                        if((buffer.get())[pos+index] != (src.buffer.get())[index]) goto goon;
                    return pos;
                }
            goon:
                pos--;
            }
            return -1;
        }

        bool Contain(const String<T>& src) const{
            return !(-1==this->find(src));
        }

        bool StartWith(const String<T>& src)const{
            return find(src) == 0;
        }

        bool EndWith(const String<T>& src)const{
            return rfind(src) + src.length  == length;
        }

        bool Empty(){
            return Empty(*this);
        }

        const String<T> ToUpperN(){
            String<T> TempString = String<T>(*this);
            T* temp = TempString.buffer.get();
            int internal = 'a' - 'A';
            while(*temp){
                if(*temp>='a' && *temp<='z')
                    *temp-=internal;
                temp++;    
            }
            return TempString;
        }

        const String<T>& ToUpper(){
            T* temp = buffer.get();
            int internal = 'a' - 'A';
            while(*temp){
                if(*temp>='a' && *temp<='z')
                    *temp-=internal;
                temp++;    
            }
            return *this;
        }

        const String<T>& ToLowwer(){
            T* temp = buffer.get();
            int internal = 'a' - 'A';
            while(*temp){
                if(*temp>='A' && *temp<='Z')
                    *temp+=internal;
                temp++;
            }
            return *this;
        }

        const String<T> ToLowwerN(){
            String<T> TempString = String<T>(*this);
            T* temp = TempString.buffer.get();
            int internal = 'a' - 'A';
            while(*temp){
                if(*temp>='a' && *temp<='z')
                    *temp+=internal;
                temp++;    
            }
            return TempString;
        }

        long ToLong()const{
            return ToL(*this);
        }

        const String<T>& replace(const String<T>& src,const String<T>& sbj){
            auto pos = -1;
            while(-1!=(pos=find(src))){
                *this = substr(0,pos)+sbj+substr(pos+src._length(),_length() -substr(0,pos)._length()-src._length());
            }
            return *this;
        }

        String<T> substr(int pos){
            return String<T>(*this,pos);
        }

        String<T> substr(int start,int length)
        {
            return String<T>(*this,start,start+length);
        }

        String<T> invert(){
            T TempBuffer[length+1];
            int start=0,end = length-1;
            while(start<length)
                TempBuffer[start++]=buffer.get()[end--];
            TempBuffer[length] = 0;
            return  (*this = TempBuffer);
        }

        bool Equal(String<T> des) const{
            return des == *this;
        }

        //KeyValueType
        template<typename type=String<T> >
        struct KeyValuePair{
            type _key;
            type _value;
            KeyValuePair(type key,type value):_key(key),_value(value){}
        };
        
        KeyValuePair<> Cut(const String<T>& split,long cutpos=-1) const{
            long count = cutpos<0?cutpos*-1:cutpos;
            long pos = 0;
            auto temp = *this;

            if(-1 == temp.find(split)){
                return KeyValuePair<>(temp,temp);
            }

            long realpos = 0;
            while(count>0){
                pos  = cutpos<0?temp.rfind(split):temp.find(split);
                if(pos == -1) break;
                temp = cutpos<0?String<T>(temp,0,pos):String<T>(temp,pos+1,temp.length-1);
                realpos = cutpos<0?pos:realpos+pos+1;
                count--;
            }
            
            realpos = cutpos<0?realpos:realpos-1;
            if(realpos == -1) return KeyValuePair<>(*this,*this);
            auto key   = String<T>(*this,0,realpos);
            auto value = String<T>(*this,realpos+split.length,length);
            return KeyValuePair<>(key,value);
        }

        typedef Contain::Linker<String<T> > CL;
        CL Split(CL slist) const
        {
            CL list;
            auto temp = *this;
            String<T> store = "";
            int index=0;
            for(;index<length;index++){
                for(int indeY = 0;indeY<slist.size;indeY++){
                    String<T> item = slist[indeY];
                    for(int indeZ=0;indeZ<slist[indeY].size && indeY+indeZ<length;indeZ++){
                        if(slist[indeY][indeZ] == temp[index+indeZ]) 
                        {
                            continue;
                        }
                        goto next;
                    }
                    list.Add(store);
                    store = "";
                    index+=item.length;
                    break;
                next:
                    ;
                }
                store+=temp[index];
            }
            //left
            if(!store.Equal("")){
                list.Add(store);
            }
            return list;
        }
        
        ///des copy last 0
        friend String<T> operator+(const String<T>& src,const String<T>& des){
            T tempbuffer[src.length+des.length+1];
            memcpy(tempbuffer,src.buffer.get(),src.size);
            memcpy(tempbuffer+src.length,des.buffer.get(),des.size+1*sizeof(T));
            return String<T>(tempbuffer);
        }

        const String<T>& operator=(const String<T>&src){
            if(*this!=src){
                buffer = src.buffer;
                length = src.length;
                size   = src.size;
            }
            return *this;
        }
        const String<T>& operator=(const String<T>&& src){
            if(*this!=src){
                buffer = move(src.buffer);
                length = src.length;
                size   = src.size;
            }
            return *this;
        }

        bool operator==(const String<T>& des)const{
            if(des.buffer==nullptr) {
                // plog("null des compare object!");
                return false;
            }
            if(size!=des.size && length!=des.length) return false;
            return Compare(*this,des) == 0;
        }
        bool operator!=(const String<T> & dec)const{
            return !(dec==*this);
        }
        const String<T>& operator+=(const String<T>& src){
            *this = *this + src;
            return *this;
        }
    };
    
    using AString = String<char>;
    using WString = String<wchar_t>;

    extern AString W2A(WString src);
    extern AString W2AS(WString src,char * buf,size_t size);
    extern WString A2W(AString src);
    extern WString A2WS(AString src,wchar_t* buf,size_t size);
    