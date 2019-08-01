#include <iostream>
#include <fstream>
#include <string>
#include "adler32.hpp"
#include "stranslate.hpp"
#include "HttpUtility.hpp"
#include <future>

using namespace std;
using namespace Utility;

unsigned long GetTickCount() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000;
}

void exec(SOCKET conn,ReceviceListner listener){
    // char buf[MAX_BUFFER * 2];
    // char sendBuf[MAX_BUFFER];
    long TotalLength = 0;
    long ContentLength = 0;
    long ChunkIndex  = 0;
    long ibret = 0;
    //last
    char buf[MAX_BUFFER];

    while(1){
        // ibret = recv(conn,buf,MAX_BUFFER,0);
        Request request;
        Request req;

        int iLastBufferSize = 0;
        int iCurrentIndex=0;
        int iTotalRecvSize = 0;

        long CurrentPackageReceviceSize =0;
        hString LastBuffer = "";
        hString CurrentBuffer = "";
        //Head
        while(1){
            ibret = recv(conn,buf,MAX_BUFFER,0);
            LastBuffer+=buf;
            if(LastBuffer.Contain("\r\n\r\n")){
                auto temp = LastBuffer.Cut("\r\n\r\n",1);
                plog("temp key:",temp._key);
                req = req.Parse(temp._key);
                LastBuffer = temp._value;
                TotalLength+=LastBuffer._length();
                break;
            }
        }

        plog("request: ",req.ToString());
        //Content
        while(1){
            if(TotalLength == req.ContentLength.ToLong()) {
                if(listener.OnHandleData)
                    listener.OnHandleData(CurrentBuffer,req,ChunkIndex++);

                #ifdef WIN32
                if(sock) closesocket(conn);
                #else
                if(conn) shutdown(conn, 2);
                #endif

                send(conn,"ACK",AString("ACK")._length(),0);
                break;
            }
            plog("TotalLength: ",TotalLength);

            ibret = recv(conn,buf,MAX_BUFFER,0);

            CurrentPackageReceviceSize += ibret;
            TotalLength+=ibret;

            if(CurrentPackageReceviceSize<MAX_BUFFER ) {
                LastBuffer += buf; 
                continue;
            }; 

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
    }
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

    while(1){
        plog("accept...");
        #ifdef WIN32
        sockConnect = accept(sock, (SOCKADDR*)&sockAddr,&size);
        #else
        sockConnect = accept(sock, (SOCKADDR*)&sockAddr,reinterpret_cast<socklen_t*>(&size));
        #endif

        plog("recv...");
        async(launch::async,exec,sockConnect,listener);

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
    start(ReceviceListner(nullptr));
    return 0;
}
