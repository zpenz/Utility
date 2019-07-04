#include "md5.h"
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <map>

#define CHUNK_SIZE 100
#define MOD_DIGEST 65521
#define MD5_SIZE 17

void FileDelete(const AString& strName,int pos,int length){
    auto file = fopen(strName.c_str(),"rb+");
    auto file2 = fopen((strName+".psync").c_str(),"w+");
    char buffer[CHUNK_SIZE];
    int size = 0;
    int TotalSize = 0;

    int count = pos/CHUNK_SIZE;
    while(count>0){
        size = fread(buffer,sizeof(char),CHUNK_SIZE,file);
        fwrite(buffer,sizeof(char),size,file2);
        TotalSize+=size;
        count--;
    }

    size = fread(buffer,sizeof(char),pos-TotalSize,file);
    fwrite(buffer,sizeof(char),size,file2);
    TotalSize+=size;

    fseek(file,length,SEEK_CUR);
    
    while((size = fread(buffer,sizeof(char),CHUNK_SIZE,file))>0){
        fwrite(buffer,sizeof(char),size,file2);
    }
    fclose(file);
    fclose(file2);
}

void FileInsert(const AString& strName,int pos,const AString& strContent){
    auto file = fopen(strName.c_str(),"rb+");
    auto file2 = fopen((strName+".psync").c_str(),"w+");
    char buffer[CHUNK_SIZE];
    int  size = 0;
    int  TotalRead = 0;

    auto RemainWrite = [&](int length){
        size = fread(buffer,sizeof(char),length,file);
        fwrite(buffer,sizeof(char),size,file2);
        
        fwrite(strContent.c_str(),sizeof(char),strContent._length(),file2);

        while((size = fread(buffer,sizeof(char),CHUNK_SIZE,file))>0){
            fwrite(buffer,sizeof(char),size,file2);
        }
    };

    int count = pos/CHUNK_SIZE;
    while(count>0){
        size = fread(buffer,sizeof(char),CHUNK_SIZE,file);
        fwrite(buffer,sizeof(char),size,file2);
        TotalRead+=size;
        count--;
    }

    RemainWrite(pos - TotalRead);

    fclose(file);
    fclose(file2);
}

void FileMarge(const AString& src,int pos,const char* diff,int size){
    FileDelete(src,pos,size);
    FileInsert(src+".psync",pos,diff);
}

struct ChunkData{
    long AValue;
    long BValue;
    const char * MValue;
    ChunkData(){}
    ChunkData(long a,long b,const char * m):AValue(a),BValue(b),MValue(m){}
    bool operator==(const ChunkData& data){
        if(AValue != data.AValue) return false;
        if(BValue != data.BValue) return false;
        if(AString(MValue) == AString(data.MValue)) return false;
        return true;
    }
};

struct FileData{
    vector<ChunkData>  ls;
    const char * filename;
    const char * filepath;
    FileData(){}
    FileData(vector<ChunkData>  l,const char * n):ls(l),filename(n){
    }
};

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

struct diff{
    int rvalue;
    int index;
    char MD5Value[MD5_SIZE];
    diff(){};
    diff(long av,long bv,const char* mv)
    {
        rvalue = bv<<16|av;
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

void SaveDiff(const AString& name,vector<diff> ls){
    auto file = fopen(name.c_str(),"w+");
    for_each(ls.begin(),ls.end(),[&](diff& item){
        // fwrite(&item,sizeof(diff),1,file);
        fwrite(&item.rvalue,sizeof(item.rvalue),1,file);
        fwrite(&item.MD5Value,sizeof(item.MD5Value),1,file);
    });
    fclose(file);
}

vector<diff> CalcFileSlideDiff(const AString& filename){
    vector<diff> data;
    auto file = fopen(filename.c_str(),"rb+");
    char buf[CHUNK_SIZE];

    int size = 0;
    int pos = 0;

    while(( size = fread(buf,sizeof(char),sizeof(buf),file))>0){
        auto MValue = MD5::Md516(buf,size);
        long AValue = 1,BValue = 0;
        for(int index=0;index<size;index++){
            AValue = (AValue + static_cast<int>(buf[index]))%MOD_DIGEST;
            BValue = (BValue + AValue)%MOD_DIGEST;
        }

        diff df = diff(AValue,BValue,MValue.c_str());
        df.index = pos;
        pos+=size;
        log(MValue);
        data.push_back(df);
    }
    log("diff size: ",data.size());
    fclose(file);
    return data;
}

vector<diff> CalcFileDiff(const AString& filename){
    vector<diff> data;

    auto file = fopen(filename.c_str(),"rb+");
    char buf[CHUNK_SIZE];

    int pos = 0;
    int size = 0;
    int startpos = 0;

    while((size = fread(buf,sizeof(char),sizeof(buf),file))>0){
        auto MValue = MD5::Md516(buf,size);
        long AValue = 1, BValue = 0; 
        for(int index=0;index<size;index++){
            AValue = (AValue+static_cast<int>(buf[index]))%MOD_DIGEST;
            BValue = (BValue + AValue)%MOD_DIGEST;
        }

        diff df = diff(AValue,BValue,MValue.c_str());
        df.index = pos++;
        data.push_back(df);

        fseek(file,++startpos,SEEK_SET);
        memset(buf,0,sizeof(buf));
    }
    fclose(file);
    return data;
}

struct range{
    int  index;
    int  length;
    int  offset;
    bool sameblock;
    range(int _index,int _length,int _offset,bool _sb = false):index(_index),length(_length),offset(_offset),sameblock(_sb){}
};

#pragma region --mark DIFF
enum class ActionType{
    ACTION_INSERT,
    ACTION_DELETE,
    ACTION_COMMON
};

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

//
#pragma region OrderSolve
void MyersDiff(AString src,AString des){
    int sizea  = src._length();
    int sizeb  = des._length();
    //index can not nagivate
    int offset = sizea+sizeb+1;
 
    vector<Prev> list(2*(sizea+sizeb),Point(0,0));
    list[1] = Point(0,0);

    vector<decltype(list)> vlist;

    for(int d=0;d<=sizea+sizeb;d++){
        printf("d=%d: \n",d);
        for(int k=-d;k<=d;k+=2){
            bool down = k==-d ||( k!= d && list[k+1+offset].pt.x>list[k+offset-1].pt.x);
            int kprev = down?k+1:k-1;

            int xStart = list[kprev+offset].pt.x;
            int yStart = xStart - kprev;

            //middle
            int xMiddle = down?xStart:xStart+1;
            int yMiddle = xMiddle - k;

            //end point
            int xEnd = xMiddle;
            int yEnd = yMiddle;

            int snake = 0;
            while(xEnd<sizea && yEnd<sizeb && src[xEnd] == des[yEnd]){
                snake++;
                xEnd++;
                yEnd++;
            }

            list[k+offset].pt.x = xEnd;

            SHOW_MESSAGE(xEnd,1);
            // SHOW_MESSAGE(snake,1);
            SHOW_MESSAGE(yEnd,1);
            //end?
            if(xEnd>=sizea && yEnd>=sizeb){
                //end...
                SHOW_MESSAGE("End...",1);
                vlist.push_back(list);
                goto solution;
            }
        }
        vlist.push_back(list);
    }

solution:
    //Solution
    vector<Snake> sl;
    Point p = Point(sizea,sizeb);
    for(int d = vlist.size()-1;p.x>0 || p.y>0;d--){
        auto v = vlist[d];

        int k = p.x - p.y;

        //end point 
        int xEnd = v[k+offset].pt.x;
        int yEnd = xEnd - k;

        bool down = (k == -d || (k!=d && v[k-1+offset].pt.x<v[k+1+offset].pt.x));

        int kPrev = down?k+1:k-1;

        //start point
        int xStart = v[kPrev+offset].pt.x;
        int ystart = xStart - kPrev;

        //mid point
        int xMid = down?xStart:xStart+1;
        int yMid = xMid - k;

        //snake
        sl.insert(sl.begin(),Snake(Point(xStart,ystart),Point(xMid,yMid),Point(xEnd,yEnd)));

        p.x = xStart;
        p.y = ystart;
    }

    for_each(sl.begin(),sl.end(),[](Snake & item){
        SHOW_MESSAGE(item.start,1);
        SHOW_MESSAGE(item.mid,1);
        SHOW_MESSAGE(item.end,1);
    });
}
#pragma endregion

struct Action{
    enum class TargetType { TargetLocal,TargetRemote};
    enum class ActionType { Normal,Insert,Delete};
    TargetType tt;
    ActionType at;
    long       pos;
    Action(TargetType _tt,ActionType _at,long _pos):tt(_tt),at(_at),pos(_pos){}
};

template<typename Type = AString>
vector<Action> Reverse(Type a,Type b,long alength,long blength){
    long sizea = alength;
    long sizeb = blength;
    vector<Action> alist = vector<Action>();
    if(sizea ==0||sizeb==0) return alist; 

    long offset = sizeb+sizea+1;

    vector<Prev> list(sizea+sizeb+offset,Point(0,0));
    vector<decltype(list)> dlist;
    list[sizea-sizeb-1+offset] = Point(sizea,sizeb+1);

    bool solution = false;
    long countd = 0;
    long delta = sizea-sizeb;

    for(long d = 0;d<sizea+sizeb && !solution;d++)
    {
        for(long k=-d+delta;k<=d+delta;k+=2){
            bool up = (k==d+delta) ||(k!=-d+delta && list[offset+k+1].pt.x > list[offset+k-1].pt.x);

            long kPrev = up ? k-1:k+1;

            long xStart = list[kPrev+offset].pt.x;
            long yStart = xStart - kPrev;

            long xEnd   = up?xStart:xStart -1;
            long yEnd   = xEnd - k;

            long distance = 0;
            while (xEnd>0 && yEnd>0 && a[xEnd-1] == b[yEnd-1])
            {
                yEnd--;
                xEnd--;
                distance++;
            }

            list[k+offset] = Point(xEnd,yEnd);
            list[k+offset].pk = kPrev;

            if (xEnd <=0 && yEnd <= 0)
            {
                dlist.push_back(list);
                countd++;

                long pk = k;
                long pyEnd = yEnd;
                long pxEnd = xEnd;

                for(long index=countd-1;index>=0;index--){
                    k = ((dlist[index])[pk+offset]).pk;

                    yEnd = ((dlist[index])[k + offset]).pt.y;
                    xEnd = ((dlist[index])[k + offset]).pt.x;

                    if(xEnd>pxEnd && yEnd>pyEnd){
                        for (long index = pyEnd+1; index <= yEnd; index++)
                        {
                            if(k<pk && index==yEnd) break;
                            log("  ",b[index-1]);
                            alist.push_back(Action(Action::TargetType::TargetRemote,
                            Action::ActionType::Normal,index-1));
                            // log("  ",index-1);
                        }
                    }

                    if(k<pk )  {
                        log("+ ",b[yEnd-1]);
                        alist.push_back(Action(Action::TargetType::TargetRemote,
                            Action::ActionType::Insert,yEnd-1));
                    }
                    // if(k<pk )  log("+ ","b ",yEnd-1);
                    if(k>pk) {
                        log("- ",a[xEnd-1]);
                        alist.push_back(Action(Action::TargetType::TargetLocal,
                            Action::ActionType::Delete,xEnd-1));
                    }
                    // if(k>pk) log("- ","a ",xEnd-1);

                    pk = k;
                    pyEnd = yEnd;
                    pxEnd = xEnd;

                    if(xEnd==sizea && yEnd==sizeb) break;
                }
                return move(alist);
            }
        }
        dlist.push_back(list);
        countd++;
    }
    return move(alist);
}
#pragma endregion

int main(int argc, char const *argv[])
{
    #pragma region --diff test
    // File Diff
    // vector<AString> fileString = vector<AString>();
    // vector<AString> file2String = vector<AString>();

    // fstream f1("2.txt",f1.binary|f1.in);
    // fstream f2("3.txt",f1.binary|f1.in);

    // char buf[CHUNK_SIZE];
    // while(!f1.eof()){
    //     memset(buf,0,sizeof(buf));
    //     f1.getline(buf,sizeof(buf));
    //     fileString.push_back(buf);
    // }

    // while(!f2.eof()){
    //     memset(buf,0,sizeof(buf));
    //     f2.getline(buf,sizeof(buf));
    //     // show_message(buf);
    //     file2String.push_back(buf);
    // }

    // Reverse(fileString, file2String,fileString.size(),file2String.size());

    #pragma endregion

    fstream f1("2.txt",f1.binary|f1.in);
    fstream f2("3.txt",f1.binary|f1.in);
    fstream fs = fstream("2.txt.marge",fstream::in | fstream::out | fstream::trunc);
    //Save
    auto ret = CalcFileSlideDiff("2.txt");
    map<int,diff> store1;
    for_each(ret.begin(),ret.end(),[&](diff & item){
        store1[item.rvalue] = item;
    });

    //roll
    auto ret2 = CalcFileDiff("3.txt");

    //calc list
    bool bDiff = false;
    vector<range> list = vector<range>();

    int  i = 0;
    
    int startpos = 0;
    int length = 0;
    while(i<ret2.size()){
        auto item = ret2[i];
        auto findret = store1.find(item.rvalue);
        if(findret!= store1.end() && strcmp(findret->second.MD5Value,item.MD5Value) == 0){
            log("index ",i);
            if(bDiff){
                f2.seekg(startpos);
                char wbuf[length];
                f2.read(wbuf,length);
                fs.write(wbuf,length);
                bDiff = false;
            }

            f1.seekg(findret->second.index);
            log("second index ",findret->second.index);
            char buf[CHUNK_SIZE];
            f1.read(buf,CHUNK_SIZE);
            int length = f1.gcount();
            fs.write(buf,length);

            i+=CHUNK_SIZE;
        }else{
            //delay
            i++;
            if(bDiff){
                length++;
            }else{
                bDiff = true;
                startpos = i;
                length = 0;
            }
        }
    }

    //other
    if(bDiff){
        f2.seekg(startpos);
        char wbuf[length];
        f2.read(wbuf,length);
        fs.write(wbuf,length);
        bDiff = false;
    }

    f1.close();
    f2.close();
    fs.close();
    return 0;
}
