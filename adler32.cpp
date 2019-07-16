    #include "adler32.hpp"
    #include "md5.h"
    #include "HttpUtility.hpp"
    #include <functional>
    #include <algorithm>
    #include <map>

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
                AValue = (AValue + static_cast<int>(buf[index]));
                BValue = (BValue + AValue);
            }

            diff df = diff(AValue,BValue,MValue.c_str(),pos);
            df.index = pos;
            pos+=size;
            data.push_back(df);
        }
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
            // auto MValue = MD5::Md516(buf,size);
            AString MValue = "";
            long AValue = 0, BValue = 0; 
            for(int index=0;index<size;index++){
                AValue = (AValue+static_cast<int>(buf[index]))%MOD_DIGEST;
                BValue = BValue + AValue%MOD_DIGEST;
            }

            diff df = diff(AValue,BValue,MValue.c_str(),pos++);
            // df.index = pos++;
            data.push_back(df);

            fseek(file,++startpos,SEEK_SET);
            memset(buf,0,sizeof(buf));
        }
        fclose(file);
        return data;
    }

    // vector<diff> CalcFileDiff_r(const AString& filename){
    //     vector<diff> data;

    //     auto file = fopen(filename.c_str(),"rb+");
    //     char buf[CHUNK_SIZE];
    //     char buf2[CHUNK_SIZE];
    //     char* bufcur = buf;
    //     char* bufbk = buf2;

    //     int pos = 0;
    //     int size = 0;

    //     size = fread(buf,sizeof(char),CHUNK_SIZE,file);
    //     int AValue = 1,BValue = 0;
    //     for(int index=0;index<size;index++){
    //         AValue = (AValue+static_cast<int>(buf[index]));
    //         BValue = (BValue + AValue);
    //     }
    //     data.push_back(diff(AValue,BValue,"",0));

    //     while((size = fread(bufbk,sizeof(char),CHUNK_SIZE,file))>0){
    //         for(int j =0;j<size;j++){
    //             AValue -= (bufcur[j]-static_cast<int>(bufbk[j]));
    //             BValue -= (CHUNK_SIZE*(bufcur[j])-AValue+1);

    //             diff df = diff(AValue,BValue,"",pos++);
    //             // df.index = pos++;
    //             data.push_back(df);
    //         }

    //         char* temp = bufcur;
    //         bufcur = bufbk;
    //         bufbk = temp;
    //     }

    //     fclose(file);
    //     return data;
    // }

    //func : fun(buf,checkvalue,index)
    //cancel push diff because of bad alloc
    vector<diff> CalcFileDiff_r(const AString& filename,std::function<void(diff&,bool&)> func){
        vector<diff> data;

        auto file = fopen(filename.c_str(),"rb+");
        char buf[CHUNK_SIZE];
        char buf2[CHUNK_SIZE];
        char* bufcur = buf;
        char* bufbk = buf2;

        int pos = 0;
        int size = 0;
        bool sameblock = false;

        fseek(file,0,SEEK_END);
        long FileLen = ftell(file);
        fseek(file,0,SEEK_SET);

    start:

        size = fread(buf,sizeof(char),CHUNK_SIZE,file);
        int AValue = 1,BValue = 0;
        for(int index=0;index<size;index++){
            AValue = (AValue+static_cast<int>(buf[index]));
            BValue = (BValue + AValue);
        }
        diff df = diff(AValue,BValue,"",pos);
        if(func) func(df,sameblock);
        if(sameblock){
            if(df.index+CHUNK_SIZE>FileLen) {
                goto end;
            }
            fseek(SEEK_SET,0,df.index+CHUNK_SIZE);
            pos+=CHUNK_SIZE;
            goto start;
        }

        while((size = fread(bufbk,sizeof(char),CHUNK_SIZE,file))>0){
            for(int j =0;j<size;j++){
                AValue -= (bufcur[j]-static_cast<int>(bufbk[j]));
                BValue -= (CHUNK_SIZE*(bufcur[j])-AValue+1);

                diff df = diff(AValue,BValue,"",pos++);
                if(func) func(df,sameblock);
                if(sameblock){
                    if(df.index+CHUNK_SIZE>FileLen) {
                        goto end;
                    }
                    fseek(SEEK_SET,0,df.index+CHUNK_SIZE);
                    goto start;
                }
            }

            char* temp = bufcur;
            bufcur = bufbk;
            bufbk = temp;
        }

        end:
        fclose(file);
        return data;
    }

    #pragma region Diff
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
                                plog("  ",b[index-1]);
                                alist.push_back(Action(Action::TargetType::TargetRemote,
                                Action::ActionType::Normal,index-1));
                                // plog("  ",index-1);
                            }
                        }

                        if(k<pk )  {
                            plog("+ ",b[yEnd-1]);
                            alist.push_back(Action(Action::TargetType::TargetRemote,
                                Action::ActionType::Insert,yEnd-1));
                        }
                        // if(k<pk )  plog("+ ","b ",yEnd-1);
                        if(k>pk) {
                            plog("- ",a[xEnd-1]);
                            alist.push_back(Action(Action::TargetType::TargetLocal,
                                Action::ActionType::Delete,xEnd-1));
                        }
                        // if(k>pk) plog("- ","a ",xEnd-1);

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

    template<typename T=AString>
    class WriteListener{
        protected:
            T data;
        public:
            virtual void WriteContent(int pos,int length,fstream & stream)=0; 
            virtual ~WriteListener(){};
    };

    class localListener:public WriteListener<>{
        public:
            localListener(const AString& _data) { data = move(_data);}

            void WriteContent(int pos,int length,fstream & stream) override{
                fstream sf = fstream(data,sf.in|sf.binary);
                sf.seekg(pos);
                plog("seek pos ",pos," length ",length);
                char buf[CHUNK_SIZE];

                for(int index=0;index<length;){
                    sf.read(buf,CHUNK_SIZE);
                    auto size = sf.gcount();
                    index+=size;
                    stream.write(buf,size);
                }

            };
    };

    void SaveRange(vector<range>& list,fstream& flocal,const char * savename){
        fstream fs = fstream(savename,fstream::in | fstream::out | fstream::trunc);
        auto headsize = sizeof(header);
        auto offset = headsize+sizeof(range)*list.size();
        header hd = header(headsize,list.size()*sizeof(range),offset);
        fs.write(reinterpret_cast<char*>(&hd),sizeof(hd));
        //write list
        for_each(list.begin(),list.end(),[&](range& rg){
                rg.offset = offset;
                fs.write(reinterpret_cast<char*>(&rg),sizeof(rg));
                offset+=sizeof(range);

                if(!rg.sameblock){
                    // offset+=rg.length;
                    flocal.seekg(rg.index);
                    char buf[CHUNK_SIZE];
                    for(int index=0;index<rg.length/CHUNK_SIZE;index++){
                        flocal.read(buf,sizeof(buf));
                        fs.write(buf,sizeof(buf));
                    }
                    flocal.read(buf,rg.length%CHUNK_SIZE);
                    fs.write(buf,rg.length%CHUNK_SIZE);
                    offset+=rg.length;
                }
        });
        //write data
        // for_each(list.begin(),list.end(),[&](range& rg){
        //     if(!rg.sameblock){
        //         flocal.seekg(rg.index);
        //         char buf[CHUNK_SIZE];
        //         for(int index=0;index<rg.length/CHUNK_SIZE;index++){
        //             flocal.read(buf,sizeof(buf));
        //             fs.write(buf,sizeof(buf));
        //         }
        //         flocal.read(buf,rg.length%CHUNK_SIZE);
        //         fs.write(buf,rg.length%CHUNK_SIZE);
        //     }
        // });
        // fs.close();
    }

    // vector<range> performMarge(const char * src,const char * dffile){
    //     fstream f1(src,f1.binary|f1.in);
    //     fstream fs = fstream(src+AString(".marge"),fstream::in | fstream::out | fstream::trunc);
    //     auto ret = LoadDiff<diff>(dffile);
        
    //     std::map<int,diff> store1;
    //     for_each(ret.begin(),ret.end(),[&](diff & item){
    //         store1[item.rvalue] = item;
    //     });

    //     //roll
    //     auto ret2 = CalcFileDiff_r(src);

    //     //calc list
    //     bool bDiff = false;
    //     vector<range> list = vector<range>();

    //     int  i = 0;
    //     int startpos = 0;
    //     int length = 0;
    //     int offset = 0;
        
    //     while(i<ret2.size())
    //     {
    //         auto localitem = ret2[i];
    //         auto remoteitem = store1.find(localitem.rvalue);

    //         if(remoteitem!= store1.end())
    //         {

    //             plog("i = ",i);
    //             f1.seekg(i);
    //             char buf[CHUNK_SIZE];
    //             f1.read(buf,CHUNK_SIZE);
    //             auto md5 = MD5::Md516(buf,CHUNK_SIZE);

    //             if(strcmp(md5.c_str(),remoteitem->second.MD5Value)==0)
    //             {
    //                 // plog(buf); return list;
    //                 if(bDiff)
    //                 {
    //                     plog("diff length ",length); 
    //                     bDiff = false;
    //                     offset+=length;
    //                     list.push_back(range(startpos, length, offset));
    //                 }

    //                 //same block
    //                 plog("localitem: ",localitem.index);
    //                 list.push_back(range(remoteitem->second.index,CHUNK_SIZE,i,true));
    //                 plog("same block index",localitem.index);
    //                 i+=CHUNK_SIZE;
    //                 offset+=length;
    //                 continue;
    //             }
    //         }
            
    //         //delay
    //         if(bDiff)
    //         {
    //             length++;
    //         }
    //         else
    //         {
    //             bDiff = true;
    //             startpos = localitem.index==0?0:localitem.index+1;
    //             length = 1;
    //         }
    //         i++;
    //     }

    //     // if last is diff
    //     if(bDiff)
    //     {
    //         bDiff = false;
    //         list.push_back(range(startpos,length,i));
    //     }

    //     //<<CHUNK_SIZE>
    //     f1.seekg(0,f1.end);
    //     auto filesize = f1.tellg();
    //     plog("i=",i);
    //     if(i<filesize)
    //         list.push_back(range(i,(int)filesize-i,i));

    //     //save range
    //     SaveRange(list,f1,AString(src)+".op");

    //     f1.close();
    //     fs.close();
    //     return list;
    // }

        vector<range> performMarge(const char * src,const char * dffile){
        fstream f1(src,f1.binary|f1.in);
        auto ret = LoadDiff<diff>(dffile);
        
        std::map<int,diff> store1;
        for_each(ret.begin(),ret.end(),[&](diff & item){
            store1[item.rvalue] = item;
        });

        int startpos = 0;
        int length = 0;
        int offset = 0;
        int lastpos = 0;

        //calc list
        bool bDiff = false;
        vector<range> list = vector<range>();

        //roll
        auto ret2 = CalcFileDiff_r(src,[&](diff& localitem,bool same){
            auto remoteitem = store1.find(localitem.rvalue);
            lastpos = localitem.index;
            if(remoteitem!=store1.end()){
                f1.seekg(localitem.index);
                char buf[CHUNK_SIZE];
                f1.read(buf,CHUNK_SIZE);
                auto md5 = MD5::Md516(buf,CHUNK_SIZE);

                if(strcmp(md5.c_str(),remoteitem->second.MD5Value)==0)
                {
                    if(bDiff)
                    {
                        plog("diff length ",length);
                        bDiff = false;
                        offset+=length;
                        list.push_back(range(startpos, length, offset));
                    }

                    //same block
                    plog("localitem: ",localitem.index);
                    list.push_back(range(remoteitem->second.index,CHUNK_SIZE,offset,true));
                    plog("same block index",localitem.index);
                    offset+=CHUNK_SIZE;
                    same = true;
                    return;
                }
            }

            //delay
            if(bDiff)
            {
                length++;
            }
            else
            {
                bDiff = true;
                startpos = localitem.index==0?0:localitem.index+1;
                length = 1;
            }
            same = false;
        });

        // if last is diff
        if(bDiff)
        {
            bDiff = false;
            list.push_back(range(startpos,length,offset));
        }

        //<<CHUNK_SIZE>
        f1.seekg(0,f1.end);
        auto filesize = f1.tellg();
        if(lastpos<filesize)
            list.push_back(range(lastpos,(int)filesize-lastpos,offset));

        //save range
        SaveRange(list,f1,AString(src)+".op");

        f1.close();
        return list;
    }

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

        SaveFile("1.diff",CalcFileSlideDiff(""));

        localListener ls =  localListener("2.txt");
        //bak 3->2 
        vector<range> oplist = performMarge("HttpUtility.cpp","1.diff");

        // fstream f1 = fstream("2.txt",f1.binary|f1.in);
        // fstream f2 = fstream("3.txt",f1.binary|f1.in);
        // fstream fs = fstream("3.marge",fstream::in | fstream::out | fstream::trunc);

        // for_each(oplist.begin(),oplist.end(),[&](range& rg){
        //     plog("index ",rg.index," offset ",rg.offset," length ",rg.length," same ",rg.sameblock);
        //     if(rg.sameblock){
        //         f1.seekg(rg.index);
        //         char buf[CHUNK_SIZE];
        //         f1.read(buf,CHUNK_SIZE);
        //         fs.write(buf,CHUNK_SIZE);
        //     }else{
        //         f2.seekg(rg.index);
        //         char buf[rg.length];
        //         f2.read(buf,rg.length);
        //         fs.write(buf, rg.length);
        //     }
        // });

        Utility::Linker<AString> params;
        params.Add("file");
        params.Add("3.txt.op");

        Utility::FormPost("http://192.168.10.23:5555/cgi-bin/xk_file_rsync_upload.cgi",params,99999,Utility::TransListener(nullptr,[](AString string){
                plog("result: ",string);
            },nullptr,nullptr),[](Utility::Request& req){
            req.OtherRecord.Add(Utility::KV("XK_JSON","{\"admin_id\":1001,\"sid\":\"PY9Su8Gu8f64EIvAso4tMMy4l7fY6f5\",\"path_name\":\"/var/share/mp/xklvm1562125210/photo/\",\"file_name\":\"ppppppppppssssssss.txt\"}"));
        });

        fstream f1 = fstream("3.txt.op",f1.binary|f1.in);
        header hd;
        f1.read((char*)&hd,sizeof(hd));
        plog(hd.RangeLength," ",hd.DataOffset," ",hd.RangeOffset," ",hd.RangeLength/sizeof(range));
        for(int index=0;index<hd.RangeLength/sizeof(range);index++){
            range item;
            f1.read((char*)&item,sizeof(item));
            plog(item.length," ",item.index," ",item.offset);
        }

        return 0;

    }