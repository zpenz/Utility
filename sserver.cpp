
#include <iostream>
#include <fstream>
#include <string>
#include "adler32.hpp"
#include "stranslate.hpp"
#include "HttpUtility.hpp"

using namespace std;
using namespace Utility;

unsigned long GetTickCount() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000;
}

bool start(ReceviceListner listener){
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
    auto ibret = ::bind(sock,reinterpret_cast<sockaddr*>(&sockAddr),sizeof(sockAddr));
    if(ibret!=0) return 0;

    listen(sock,5);

    SOCKET sockConnect = 0;
    int size = sizeof(SOCKADDR);
    fstream fo;

    //last
    char buf[MAX_BUFFER];

    long CurrentPackageReceviceSize =0;
    hString LastBuffer = "";
    hString CurrentBuffer = "";

    while(1){
        plog("accept...");
       
        #ifdef WIN32
        sockConnect = accept(sock, (SOCKADDR*)&sockAddr,&size);
        #else
        sockConnect = accept(sock, (SOCKADDR*)&sockAddr,reinterpret_cast<socklen_t*>(&size));
        #endif

        plog("recv...");
        Request request;

        long TotalLength = 0;
        long ContentLength = 0;
        long ChunkIndex  = 0;
        Request req;
        //Head
        while(1){
            ibret = recv(sockConnect,buf,MAX_BUFFER,0);
            LastBuffer+=buf;
            if(LastBuffer.Contain("\r\n\r\n")){
                auto temp = LastBuffer.Cut("\r\n\r\n",1);
                req = Request::Parse(temp._key);
                LastBuffer = temp._value;
                TotalLength+=LastBuffer._length();
                break;
            }
        }
        plog("Content-Length :",req.ContentLength.ToLong());
        //Content
        while(1){
            memset(buf,0,MAX_BUFFER);
            if(TotalLength >= req.ContentLength.ToLong()) {
                plog("==");
                if(listener.OnHandleData)
                    listener.OnHandleData(CurrentBuffer,req,ChunkIndex++);

                #ifdef WIN32
                if(sock) closesocket(sockConnect);
                #else
                if(sockConnect) shutdown(sockConnect, 2);
                #endif

                break;
            }
            plog("TotalLength :",TotalLength);

            ibret = recv(sockConnect,buf,MAX_BUFFER,0);

            CurrentPackageReceviceSize += ibret;
            TotalLength+=ibret;
            LastBuffer += buf;

            if(CurrentPackageReceviceSize<MAX_BUFFER ) {
                continue;
            };
            
            plog("LastBuffer Length: ",strlen(buf)," Length ",AString(buf)._length()," LastBuffer ",LastBuffer._length(),"buffer ",LastBuffer);
            plog("CurrentPackageReceviceSize: ",CurrentPackageReceviceSize);
            CurrentBuffer = LastBuffer.substr(0,MAX_BUFFER);
            CurrentPackageReceviceSize = LastBuffer._length()-MAX_BUFFER;
            LastBuffer = LastBuffer.substr(MAX_BUFFER-1);

            if(listener.OnHandleData)
                listener.OnHandleData(CurrentBuffer,req,ChunkIndex++);
            // plog("ContentLength: ",ibret);
  
        // if(ibret<=0){
        //     plog("send ...");
        //     ibret = send(sockConnect,AString("ACK FROM SERVER").c_str(),AString("ACK FROM SERVER")._length(),0);
        //     plog("send length",ibret);
        // }

    }
    #ifdef WIN32
    if(sock) closesocket(sock);
    WSACleanup();
    #else
    if(sock) shutdown(sock, 2);
    #endif
    return 0;
    }
}

int main(void){
    ReceviceListner listen;
    start(listen);
}
