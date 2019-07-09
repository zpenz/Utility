    #pragma once

    #include <vector>
    #include "pString.hpp"

    #define CHUNK_SIZE 1024
    #define MOD_DIGEST 65521
    #define MD5_SIZE 17

    struct range{
        int  index;
        int  length;
        int  offset;
        bool sameblock;
        range(int _index,int _length,int _offset,bool _sb = false):index(_index),length(_length),offset(_offset),sameblock(_sb){}
        range(){}
    };

    struct diff{
        char MD5Value[MD5_SIZE];
        int index = 0;
        int rvalue;
        diff(){};
        diff(int av,int bv,const char* mv)
        {
            av = av % MOD_DIGEST;
            bv = bv % MOD_DIGEST;
            rvalue = (bv<<16) | av;
            // printf("rvalue %0x\n",rvalue); 
            memset(MD5Value,0,sizeof(MD5Value));
            memcpy(MD5Value,mv,MD5_SIZE);
        };

        bool operator==(const diff & df){
            if(rvalue != df.rvalue) return false;
            return strcmp(MD5Value,df.MD5Value) == 0;
        }

        friend ostream& operator<<(ostream &of,diff  df){
            of<<"rvalue : "<<df.rvalue<<" md5 : "<<df.MD5Value<<"\n";
            return of;
        }

        friend istream& operator>>(istream &in,diff df){
            in>>df.rvalue>>df.MD5Value;
            return in;
        }
    };

    template<typename T>
    void SaveFile(const AString& name,vector<T> ls){
        auto file = fopen(name.c_str(),"w+");
        for_each(ls.begin(),ls.end(),[&file](T& item){
            fwrite(&item,sizeof(T),1,file);
        });
        fclose(file);
    }

    template<typename T>
    vector<T> LoadDiff(const AString& name){
        auto file = fopen(name.c_str(),"rb");
        vector<T> ls;
        T obj;
        int size = 0;
        while((size = fread(&obj,sizeof(T),1,file))>0){
            for(int index =0;index<size;index++)
                ls.push_back(obj);
        }
        fclose(file);
        return ls;
    }

    extern vector<diff> CalcFileSlideDiff(const AString& filename);
    extern vector<diff> CalcFileDiff(const AString& filename);
    extern vector<diff> CalcFileDiff_r(const AString& filename);


    struct Point{
        int x;
        int y;
        Point(int _x,int _y):x(_x),y(_y){};
        friend ostream & operator<<(ostream &out,Point pt){
            out<<"x:"<<pt.x<<" y:"<<pt.y;
            return out;
        }
        friend istream & operator>>(istream &in,Point pt){
            in>>pt.x>>pt.y;
            if(!in){
                pt = Point(0,0);
            }
            return in;
        }
    };

    enum class ActionType{
        ACTION_INSERT,
        ACTION_DELETE,
        ACTION_COMMON
    };

    struct Prev{
        Point pt;
        long pk;
        long k;
        ActionType type;
        Prev* last;
        Prev(Point _pt):pt(_pt){ last = nullptr;}
    };

    struct Snake{
        Point start;
        Point mid;
        Point end;
        Snake(Point _start,Point _mid,Point _end):start(_start),mid(_mid),end(_end){};
    };

    struct Action{
        enum class TargetType { TargetLocal,TargetRemote};
        enum class ActionType { Normal,Insert,Delete};
        TargetType tt;
        ActionType at;
        long       pos;
        Action(TargetType _tt,ActionType _at,long _pos):tt(_tt),at(_at),pos(_pos){}
    };

    extern void MyersDiff(AString src,AString des);


