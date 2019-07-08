
#include <iostream>
#include <fstream>
#include <string>
#include "adler32.hpp"
#include "stranslate.hpp"

using namespace std;

unsigned long GetTickCount() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000;
}

int main(void){

    #ifdef WIN32
        WSAData wsa;
        if (::WSAStartup(MAKEWORD(1,1),&wsa) != 0)
        {
            cout<<"WSAStartup error"<<endl;
            return 0;
        }
    #endif

    auto sock = socket(AF_INET,SOCK_STREAM,0);
    if(!sock) return 0;

    sockaddr_in sockAddr;
    memset(&sockAddr,0,sizeof(sockAddr));
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons(9000);
    #ifdef WIN32
    sockAddr.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
    #else
    sockAddr.sin_addr.s_addr = INADDR_ANY;
    #endif

    auto iRuse = "1";
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, iRuse, sizeof(iRuse));
    int nRecvBuf=8*1024;
    setsockopt(sock,SOL_SOCKET,SO_RCVBUF,(const char*)&nRecvBuf,sizeof(int));
    auto ibret = bind(sock,reinterpret_cast<sockaddr*>(&sockAddr),sizeof(sockAddr));
    if(ibret!=0) return 0;

    listen(sock,5);

    SOCKET sockConnect = 0;
    int size = sizeof(SOCKADDR);

   
    fstream fs,fo;

    int totalsize=0;
    //last
    char buf[MAX_BUFFER * 2];
    char sendBuf[MAX_BUFFER];
    char lastBuffer[MAX_BUFFER*2];
    char tempBuffer[MAX_BUFFER];
    bool bFirst = true;
    int iLastBufferSize = 0;
    int iCurrentIndex=0;
    int iTotalRecvSize = 0;

    auto copy = [&](int iBufCount){
         cout<<"copy"<<endl;
         char temp[MAX_BUFFER * 2];
         memset(temp,0,MAX_BUFFER*2);
         memcpy(temp,lastBuffer,iLastBufferSize);
         memcpy(&temp[iLastBufferSize],buf,iBufCount);
         memset(buf,0,MAX_BUFFER*2);
         memcpy(buf,temp,iBufCount+iLastBufferSize);    
    };
    
    auto log = [&fs](const char* message){
        #ifdef LOG_CONTENT
        fs<<message<<endl;
        #endif
    };

    DWORD dStartTime = GetTickCount();
    while(1){
        log("accept...");
       
        #ifdef WIN32
        sockConnect = accept(sock, (SOCKADDR*)&sockAddr,&size);
        #else
        sockConnect = accept(sock, (SOCKADDR*)&sockAddr,reinterpret_cast<socklen_t*>(&size));
        #endif

        log("recv...");

        fs.open("translate.log",ios::out|ios::binary);

        dStartTime = GetTickCount();
        while(1){
            ibret = recv(sockConnect,buf,MAX_BUFFER,0);
            if(ibret<0){              
                #ifdef WIN32
                int iError_code = WSAGetLastError();
                cout<<"Error_code:"<<iError_code<<endl;
                #endif
                exit(-1);
            }
            iCurrentIndex = 0;
            
            log("size: ",ibret);
            fs << "SIZE: " << ibret << endl;

            iTotalRecvSize+=ibret;
            //Megra
            if (!bFirst)
            {
                copy(ibret);
            }

            auto headsize=((PACKAGE_HEAD*)buf)->package_size;
            auto datakind=((PACKAGE_HEAD*)buf)->datakind;
            cout<<"HEAD_SIZE:"<<headsize<<endl;
            fs << "HEAD_SIZE:" << headsize << endl;
            if(headsize==0) continue;
 
            int iPos = 0;
            while (iPos < ibret + iLastBufferSize)
            {
                if (datakind == 1 )
                {
                    if (iPos + sizeof(FILE_PACKAGE) > ibret + iLastBufferSize)
                        break;
                    //write
                    FILE_PACKAGE *file_package = nullptr;
                    file_package = (FILE_PACKAGE *)&buf[iPos];
                    fs << "Package Start-------------------------------------" << endl;
                    fs << "Recv_Data:"
                       << "\n";
                    fs << "file_index: " << file_package->index << endl;
                    fs << "file_name: " << file_package->file_name << endl;
                    fs << "file_size: " << file_package->file_info_size << endl;
                    fs << "package_size: " << file_package->head.package_size << endl;
                    fs<<  "total_size: "<<file_package->file_total_size<<endl;
                    fs << "package_content: " << file_package->package_content << endl;
                    fs << "package End---------------------------------------" << endl;
                    if(file_package->head.package_size<0 || file_package->head.package_size>sizeof(FILE_PACKAGE)) {
                        cout<<"Error: Parse Error!"<<endl;
                        exit(-1);
                    }
                    totalsize+=file_package->file_info_size;
                    if(file_package->index ==0){
                        
                        cout<<string(file_package->file_name).substr(string(file_package->file_name).rfind("/")+1)<<endl;
                        fo.open(
                            string(file_package->file_name).substr(string(file_package->file_name).rfind("/")+1),
                            ios::out|ios::binary);
                    }
                    fo.write(file_package->package_content, file_package->file_info_size);

                    iPos += sizeof(FILE_PACKAGE);
                }
                else if (datakind == 0 )
                {
                    if (iPos + sizeof(TRANSFORM_STATE) > ibret + iLastBufferSize)
                        break;
                    TRANSFORM_STATE* pState = (TRANSFORM_STATE*)&buf[iPos];
                    if(strcmp(pState->message,"final")==0) goto goon;
                    iPos += sizeof(TRANSFORM_STATE);
                }
                datakind = ((PACKAGE_HEAD *)&buf[iPos])->datakind;
                fs << "iPos: " << iPos << endl;
                fs<<"datakind: " <<datakind<<endl;
                //error
                // iCurrentIndex++;
                // if (iCurrentIndex>100) {
                //     cout<<"rep"<<endl;
                //     exit(-1);
                // }
            }
            //remain
            if (iPos < ibret + iLastBufferSize){
                iLastBufferSize = iLastBufferSize + ibret - iPos;
                fs<<"iLastBufferSize: "<<iLastBufferSize<<endl;
                memset(lastBuffer,0,MAX_BUFFER*2);
                memcpy(lastBuffer, &buf[iPos], iLastBufferSize);
                bFirst = false;
            }else{
                memset(lastBuffer, 0, MAX_BUFFER * 2);
                iLastBufferSize = 0;
                bFirst = true;
            }
      
            // memset(sendBuf,0,MAX_BUFFER);
            // TRANSFORM_STATE state;
            // memset(&state,0,sizeof(TRANSFORM_STATE));
            // memcpy(state.message,"receive",strlen("receive"));
            // send(sockConnect,reinterpret_cast<char*>(&state),sizeof(state),0);
            cout<<endl;
            fs << endl;

            //clear buf
            memset(buf,0,MAX_BUFFER*2);
            if(strcmp(((TRANSFORM_STATE*)buf)->message,"final")==0) break;
        }
        
        goon:;
        fs<<"total size: "<<totalsize<<endl;
        fs<<"total recv: "<<iTotalRecvSize<<endl; 
        cout<<"speed time: "<<(GetTickCount()-dStartTime)*0.001<<endl;
        memset(buf,0,MAX_BUFFER*2);
        memset(lastBuffer,0,MAX_BUFFER*2);
        iLastBufferSize = 0;
        bFirst = true;
        totalsize = 0; 
        iTotalRecvSize = 0;
        fo.close();
        fs.close();   
         #ifdef WIN32
         if(sock) closesocket(sockConnect);
         #else
         if(sockConnect) shutdown(sockConnect, 2);
         #endif
    }
    #ifdef WIN32
    if(sock) closesocket(sock);
    WSACleanup();
    #else
    if(sock) shutdown(sock, 2);
    #endif
    return 0;
}
