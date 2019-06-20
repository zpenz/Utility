#include "HttpUtility.hpp"
#ifdef WIN32
#include <WinSock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>

#endif
#include <iostream>
#include <fstream>

using SOCKET = int;
using SOCKADDR = sockaddr;
using DWORD = unsigned int;

AString FormPost(const AString& url,const AString& data,long timeout){
    #ifdef WIN32
    WSAData wsa;
    if (::WSAStartup(MAKEWORD(1,1),&wsa) != 0)
    {
        std::cout<<"WSAStartup error"<<std::endl;
        return 0;
    }
    #endif                                          
    auto sock = socket(AF_INET,SOCK_STREAM,0);
    if(!sock) return -2;

    sockaddr_in sockAddr;
    memset(&sockAddr,0,sizeof(sockAddr));
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_addr.s_addr = inet_addr(url.c_str());
    sockAddr.sin_port = htons(80);

    setsockopt(sock,SOL_SOCKET,SO_TIMESTAMP,&timeout,sizeof(timeout));

    auto ibret = connect(sock,reinterpret_cast<sockaddr*>(&sockAddr),sizeof(sockAddr));
    if(ibret!=0) return -1;  

    std::fstream reader;
    reader.open(data.c_str(),std::ios::in|std::ios::binary);

    reader.seekg(0,reader.end);
    size_t fileSize = reader.tellg();
    reader.seekg(0,reader.beg);

    AString header = "POST "+url+" HTTP/1.1\r\n";
    header += "host: "+url+"\r\n";
    AString boundary = "--myboundary";
    header += "Content-Type: multipart/form-data; boundary="+boundary+"\r\n";
    header += "Content-Length: "+AString((long)fileSize)+"\r\n";
    header += "Connection: close\r\n\r\n";
    header += boundary+"\r\n";
    header += "Content-Type: application/octet-stream\r\n";
    header += "Content-Disposition: form-data; name=\"file\";filename=\"myfile.txt\"\r\n";
    header += "Content-Transfer-Encoding: 8bit\r\n\r\n";

    ibret = send(sock,header.c_str(),header._length(),0);
    char tempbuf[4096];

    while(1){
        reader.read(tempbuf,sizeof(tempbuf));
        auto size = reader.gcount();
        ibret = send(sock,tempbuf,size,0);
        if(reader.eof()) break;
    }
    reader.close();
    AString footer = "\r\n--"+boundary+"--\r\n";
    ibret = send(sock,footer,footer._length(),0);

    AString RequestResult = "";
    while(1){
        memset(tempbuf,0,sizeof(tempbuf));
        ibret = recv(sock,tempbuf,sizeof(tempbuf),0);
        if(ibret ==0 ) break;
        RequestResult+=tempbuf;
    }

    #ifdef WIN32
    closesocket(sock);
    WSACleanup();
    #else
    if(sock) shutdown(sock,2);
    #endif

    return true;
}