#include "md5.h"
#include <stdio.h>
#include <vector>
#include <algorithm>

#define CHUNK_SIZE 4096

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
void SaveDiff(const AString& name,vector<T> ls){
    auto file = fopen(name.c_str(),"w+");
    for_each(ls.begin(),ls.end(),[&](T& item){
        fwrite(&item,sizeof(T),1,file);
    });
    fclose(file);
}

void SaveDiff(const AString& name,vector<FileData> ls){
    auto file = fopen(name.c_str(),"w+");
    for_each(ls.begin(),ls.end(),[&](FileData & item){
        fwrite(&item,sizeof(FileData),1,file);
    });
    fclose(file);
}

template<typename T>
vector<T> LoadDiff(const AString& name){
    auto file = fopen(name.c_str(),"rb");
    vector<T> ls;
    T buf[4096];
    int size = 0;
    while((size = fread(buf,sizeof(T),4096,file))>0){
        for(int index =0;index<size;index++)
            ls.push_back(buf[index]);
    }
    fclose(file);
    return ls;
}

FileData CalcFileDiff(const AString& filename){
    FileData data;
    data.ls.clear();

    data.filename = filename.c_str();

    auto file = fopen(filename.c_str(),"rb+");
       
    char buf[CHUNK_SIZE];
    long AValue = 0;
    long BValue = 0;
    int size = 0;
    int headValue = 0;
    size = fread(buf,sizeof(char),sizeof(buf),file);
    for(int index = 0;index<size;index++){
        AValue+=static_cast<int>(buf[index]);
        BValue+=AValue;
        auto MValue = MD5::Md5(buf,CHUNK_SIZE);
        
    }

    //rolling
    char temp = 0;
    while((size = fread(&temp,sizeof(char),1,file))>0){
        headValue = buf[0];
        for(int index = 0;index<CHUNK_SIZE;index++){
            buf[index] = buf[index+1];
        }
        buf[CHUNK_SIZE-1] = temp; 
        auto MValue = MD5::Md5(buf,CHUNK_SIZE);

        AValue -= headValue;
        AValue += buf[CHUNK_SIZE-1];
        BValue -= CHUNK_SIZE*headValue;
        BValue += AValue;

        // printf("AValue: %ld\n",AValue);
        // printf("BValue: %ld\n",BValue);
        // printf("MValue: %s\n",MValue.c_str());

        data.ls.push_back(ChunkData(AValue,BValue,MValue.c_str()));
        // printf("adler32:%ld\n",BValue%65521<<16|AValue%65521);
        // SHOW_MESSAGE("push",1);
    }
    
    // for_each(data.ls.begin(),data.ls.end(),[](ChunkData & item){
    //     printf("AValue: %ld\n",item.AValue);
    // });

    fclose(file);
    return data;
}

struct range{
    int start;
    int length;
    range(int st,int lg):start(st),length(lg){}
    range(){}
};

// AString diff(AString e,AString f,int i =0,int j=0){
//     int N = e._length();
//     int M = f._length();
//     int L = N+M;
//     int Z = 2*min(N,M)+2;

//     if(N>0 && M >0){
//         int w = N-M;
//         vector<int> g = vector<int>(Z);
//         vector<int> p = vector<int>(Z);
//         for (int h=0;h<(L/2+L%2+1);h++)
//         {
//             for(int r =0;r<2;r++){
//                 auto c = r==0?g:p;
//                 auto d = r==0?p:g;
//                 auto o = r==0?1:0;
//                 auto m = r==0?1:-1;

//                 for(int k = -(h-2*max(0,h-M));k<h-2*max(0,h-N)+1;k+=2){
//                     auto a = (k==-h || k!=h && c[(k-1)%Z]<c[(k+1)%Z])?c[(k+1)%Z]:c[(k-1)%Z]+1;
//                     auto b = a -k;
//                     auto s = a;
//                     auto t = b;
//                     while(a<N && b<M && e.At((1-o)*N+m*a+(o-1)) == f.At((1-o)*M+m*b+(o-1))){
//                          a++;
//                          b++;   
//                     } 
//                     c[k%Z] = a;
//                     auto z = -(k-w);
//                     if(L%2 == o && z>=-(h-o) && z<=h-o && c[k%z]+d[z%Z] >=N){
//                         auto D = o==1?2*h-1:2*h;
//                         auto x = o==1?s:N-a;
//                         auto y = o==1?t:M-b;
//                         auto u = o==1?a:N-s;
//                         auto v = o==1?b:M-t;
//                         if(D>1 || (x!=u && y!=v)){
//                             return diff(e.substr(0,x),f.substr(0,y),i,j)+diff(e.substr(u,N-u),f.substr(v,M-v),i+u,j+v); 
//                         }else if(M>N){
//                             return diff("",f.substr(N,M-N),i+N,j+N); 
//                         }else if(M<N){
//                             return diff(e.substr(M,N-M),"",i+M,j+M);
//                         }else{
//                            return "null"; 
//                         }
                        
//                     }
//                 }
//             }
//         }
        
//     }else if(N>0){
//         AString a = "";
//         for(int n=0;n<N;n++){
//             a+="old_position: "+AString(n+i);
//         }
//         return "delete"+a;
//     }
//     else{
//         AString b = "";
//         for(int n=0;n<N;n++){
//             b+="old_position: "+AString(i)+" new_position:";
//             b+=j+n;
//         }
//         return "insert"+b;
//     }
// }

enum class ActionType{
    ACTION_INSERT,
    ACTION_DELETE,
    ACTION_COMMON
};

struct Diff{
    ActionType type;
    int pos;
    Diff(ActionType _type,int _pos):type(_type),pos(_pos){}
};

struct Prev{
    int x;
    Prev(int _x):x(_x){}
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

struct Snake{
    Point start;
    Point mid;
    Point end;
    Snake(Point _start,Point _mid,Point _end):start(_start),mid(_mid),end(_end){};
};

void MyersDiff(AString src,AString des){
    int sizea  = src._length();
    int sizeb  = des._length();
    //index can not nagivate
    int offset = sizea+sizeb+1;
 
    vector<Prev> list(2*(sizea+sizeb),0);
    list[1] = 0;

    vector<decltype(list)> vlist;

    for(int d=0;d<=sizea+sizeb;d++){
        printf("d=%d: \n",d);
        for(int k=-d;k<=d;k+=2){
            bool down = k==-d ||( k!= d && list[k+1+offset].x>list[k+offset-1].x);
            int kprev = down?k+1:k-1;

            int xStart = list[kprev+offset].x;
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

            list[k+offset].x = xEnd;

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
        int xEnd = v[k+offset].x;
        int yEnd = xEnd - k;

        bool down = (k == -d || (k!=d && v[k-1+offset].x<v[k+1+offset].x));

        int kPrev = down?k+1:k-1;

        //start point
        int xStart = v[kPrev+offset].x;
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

void Reverse(AString a,AString b){
    int sizea = a._length();
    int sizeb = b._length();

    int offset = sizea+sizeb;

    vector<Prev> list(sizea+sizeb+offset,0);
    vector<decltype(list)> dlist;
    list[-1 + offset] = -1;

    bool solve = false;

    for(int d=0;d<=sizea+sizeb && !solve;d++){
        printf("d=%d: \n",d);
        for(int k=-d;k<=d;k+=2){

            //Prev .x represent y value
            bool up = k==d ||(k!=-d && list[offset+k+1].x - (k+1) <list[offset+k-1].x-(k-1));

            int kPrev = up ? k-1:k+1;
            int yStart = list[kPrev+offset].x;
            int xStart = yStart - kPrev;

            int yMid   = up?yStart+1:yStart;
            int xMid   = yMid - k;

            int yEnd   = yMid;
            int xEnd   = xMid;

            int distance = 0;
            while (xEnd<sizea && yEnd<sizeb && a[sizea-1-xEnd] == b[sizeb-1-yEnd])
            {
                yEnd++;
                xEnd++;
                distance++;
            }

            SHOW_MESSAGE(xEnd, 1);
            SHOW_MESSAGE(yEnd, 1);

            list[k+offset] = yEnd;

            //solution
            if(xEnd>=sizea && yEnd>=sizeb){
                solve = true;
            }
        }
        dlist.push_back(list);
    }

    //solve
    vector<Snake> sl;

    Point pt = Point(sizea,sizeb);
    for(int d = dlist.size()-1;pt.x>0 || pt.y>0;d--){
        int k = pt.y-pt.x;
        auto v = dlist[d];
        bool up = k==d ||(k!=-d && v[offset+k-1].x-(k-1) > v[offset+k+1].x-(k+1));

        int yEnd = v[k+offset].x;
        int xEnd = yEnd - k;

        int kPrev   = up?k-1:k+1;

        int yStart = v[kPrev + offset].x;
        int xStart = yStart - kPrev;

        int yMiddle = up?yStart + 1:yStart; 
        int xMiddle = yMiddle - k;

        sl.push_back(Snake(Point(xStart,yStart),Point(xMiddle,yMiddle),Point(xEnd,yEnd)));

        pt.x = xStart;
        pt.y = yStart;
    }

    for_each(sl.rbegin(), sl.rend(), [](Snake &item) {
        SHOW_MESSAGE(item.start, 1);
        SHOW_MESSAGE(item.mid, 1); 
    }); 
}

int main(int argc, char const *argv[])
{
    // vector<FileData> fl;
    // vector<ChunkData> diff;
    // vector<range> rangelist;
    // // auto ret1 = CalcFileDiff("1.txt");          //old

    // FileData ret1 = CalcFileDiff("1.txt");
    // FileData ret2 = CalcFileDiff("1.txt.diff");
    // // auto ret2 = CalcFileDiff("1.txt.diff");     //new

    // int indexX = 0,indexY = 0;
    // range rg;
    // bool  havediff =true;
    // int start = 0;
    // int length = 0;
    
    // SHOW_MESSAGE(diff("abcdef","acf"),1);

    Reverse("abcabba", "cbabac");

    // for_each(rangelist.begin(),rangelist.end(),[&](range & item){
    //     SHOW_MESSAGE(item.start,1);
    //     SHOW_MESSAGE(item.length,1);
    // });
    
    // SaveDiff<FileData>("sync.diff",fl);
    // FileMarge("1.txt",2,"123",4096);
    // auto ret = LoadDiff<FileData>("sync.diff");
    return 0;
}
