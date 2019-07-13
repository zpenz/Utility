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

namespace Utility
{

Request::Request(hString url){
    auto ret = CutUrl(url);
    Origin = ret._key;
    ActionAddress = ret._value;
    Host = Origin.Contain("http")?Origin.Cut("/",3)._value:Origin;
    port = Host.Contain(":")?Host.Cut(":")._value.ToLong():80;
}

hString Request::ToString(){
    hString 
    header  = "POST "+ActionAddress+" "+HttpVersion+"\r\n";
    header += Host.Empty()?"":"Host: "+Host+"\r\n";
    header += ContentType.Empty()?"":"Content-Type: "+ContentType+"\r\n";
    header += ContentLength.Empty()?"":"Content-Length: "+ContentLength+"\r\n";
    header += Connection.Empty()?"":"Connection: "+Connection+"\r\n";
    header += Accept.Empty()?"":"Accept: "+Accept+"\r\n";
    header += AcceptEncoding.Empty()?"":"Accept-Encoding: "+AcceptEncoding+"\r\n";
    header += AcceptLanguage.Empty()?"":"Accept-Length: "+AcceptLanguage+"\r\n";
    header += Cookie.Empty()?"":"Cookie: "+Cookie+"\r\n";
    header += Origin.Empty()?"":"Origin: "+Origin+"\r\n"; 
    header += Referer.Empty()?"":"Referer: "+Referer+"\r\n"; 
    header += UserAgent.Empty()?"":"User-Agent: "+UserAgent+"\r\n"; 
    header += Range.Empty()?"":"Range: "+Range+"\r\n"; 
    header += XRequestedWith.Empty()?"":"X-Requested-With:"+XRequestedWith+"\r\n";
    for(int index=0;index<OtherRecord.size;index++){
        header += OtherRecord[index]._key+": "+OtherRecord[index]._value+"\r\n";
        // if(index!=OtherRecord.size-1) header += "\r\n";
    }
    header+="\r\n";
    return header;
}

Request Request::Parse(const AString& buf) {
    Request req;

    auto ret = buf.Split("\r\n");
    for(int index=0;index<ret.size;index++){
        auto item = ret[index];
        if(item.StartWith("POST") || item.StartWith("GET")){
            auto first = item.Split(" ");
            if(first.size<3){
                plog("error parse post/get");
                return req;
            }
            req.Methon = first[0];
            req.ActionAddress = first[1];
            req.HttpVersion = first[2];
        }
        else{
            auto keyvalue = item.Cut(":",1);
            auto value = keyvalue._value;
            if(keyvalue._key.StartWith("Host")){
                req.Host = value;
            }else
            if(keyvalue._key.StartWith("Connection")){
                req.Connection = value;
            }else
            if(keyvalue._key.StartWith("Content-Length")){
                req.ContentLength = value;
            }else
            if(keyvalue._key.StartWith("Accept")){
                req.Accept = value;
            }else
            if(keyvalue._key.StartWith("Origin")){
                req.Origin = value;
            }else
            if(keyvalue._key.StartWith("X-Requested-With")){
                req.XRequestedWith = value;
            }else
            if(keyvalue._key.StartWith("Content-Type")){
                req.ContentType = value;
                if(value.Contain("boundary")){
                    req.Boundary = value.Cut("=")._value;
                }
            }else
            if(keyvalue._key.StartWith("Referer")){
                req.Referer = value;
            }else
            if(keyvalue._key.StartWith("Accept-Encoding")){
                req.AcceptEncoding = value;
            }else
            if(keyvalue._key.StartWith("Accept-Language")){
                req.AcceptLanguage = value;
            }else{
                req.OtherRecord.Add(KV(keyvalue._key,value));
            }
        }
    }
    return req;
}

hString::KeyValuePair<hString> CutUrl(const hString& url){
    if(url.StartWith("http"))
        return url.Cut("/",3);
    return  url.Cut("/",1);
}

hString FormPost(const hString& url,Linker<hString> list,long timeout,TransListener listener,std::function<void(Request& req)> OtherSetting){
    if(list.size %2 !=0) return -3;
    #ifdef WIN32
    WSAData wsa;
    if (::WSAStartup(MAKEWORD(1,1),&wsa) != 0)
    {
        std::cout<<"WSAStartup error"<<std::endl;
        return 0;
    }
    #endif                                          
    auto sock = socket(AF_INET,SOCK_STREAM,0);
    if(!sock) 
        {if(listener.OnError!=nullptr) listener.OnError("创建socket失败!"); return "";}
    hString boundary = "--xkboundary";

    auto urlkeyvalue = CutUrl(url);
    int port = urlkeyvalue._key.Contain(":")?hString::ToL(urlkeyvalue._key.Cut(":")._value):80;
    auto address = urlkeyvalue._key.Contain(":")?urlkeyvalue._key.Cut(":")._key:urlkeyvalue._key;
    address = address.Contain("http")?address.Cut("/",3)._value:address;

    sockaddr_in sockAddr;
    memset(&sockAddr,0,sizeof(sockAddr));
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_addr.s_addr = inet_addr(address.c_str());
    sockAddr.sin_port = htons(port);

    setsockopt(sock,SOL_SOCKET,SO_TIMESTAMP,&timeout,sizeof(timeout));

    auto ibret = connect(sock,reinterpret_cast<sockaddr*>(&sockAddr),sizeof(sockAddr));
    if(ibret!=0) 
        {if(listener.OnError!=nullptr) listener.OnError("连接到服务器失败!"); return "";}

    std::fstream reader;

    long length = 0;
    long filesize = 0;
    long sendlength = 0;
    hString builder;
    for(int index=0;index<list.size;index+=2){
        if(list[index].Equal("file")){
            auto filename = list[index+1].Cut("/",-1)._value;
            builder += "--"+boundary+"\r\nContent-Disposition: form-data; name= \"file\"; filename=\""+filename+"\"\r\nContent-Type: application/octet-stream\r\n\r\n";
            reader.open(list[index+1].c_str(),std::ios::in|std::ios::binary);
            reader.seekg(0,reader.end);
            size_t filesize = reader.tellg();
            length  +=filesize;
            reader.seekg(0,reader.beg);
            reader.close();
            builder +="\r\n";
        }else{
            builder += "--"+boundary+"\r\nContent-Disposition: form-data; name=\""+list[index]+"\"\r\n\r\n";
            builder += list[index+1]+"\r\n";
        }
    }
    builder+="--"+boundary+"--\r\n\r\n";
    length+=builder._length();
    show_message(builder);
    builder = "";

    Request request(url);
    request.ContentLength = length;
    request.ContentType = "multipart/form-data; boundary="+boundary;
    request.Connection = "close";
    if(OtherSetting!=nullptr) OtherSetting(request);

    auto header = request.ToString();
    show_message("header: ",header);
    ibret = send(sock,header.c_str(),header._length(),0);

    char tempbuf[TRANSLATE_SIZE];
    for(int index=0;index<list.size;index+=2){
        if(list[index].Equal("file")){
            auto filename = list[index+1].Cut("/",-1)._value;
            builder += "--"+boundary+"\r\nContent-Disposition: form-data; name= \"file\"; filename=\""+filename+"\"\r\nContent-Type: application/octet-stream\r\n\r\n";
            ibret = send(sock,builder.c_str(),builder._length(),0); 
            sendlength+=ibret;
            builder = "";

            reader.open(list[index+1].c_str(),std::ios::in|std::ios::binary);
            while(1){
                memset(tempbuf,0,sizeof(tempbuf));
                reader.read(tempbuf,sizeof(tempbuf));
                auto size = reader.gcount();
                
                ibret = send(sock,tempbuf,size,0);
                sendlength+=ibret;
                if(listener.OnChange!=nullptr) listener.OnChange(sendlength,length);
                if(reader.eof()) break;
            }
            reader.close();

            builder +="\r\n";
            ibret = send(sock,builder.c_str(),builder._length(),0); 
            sendlength+=ibret;
            builder = "";
        }else{
            builder += "--"+boundary+"\r\nContent-Disposition: form-data; name=\""+list[index]+"\"\r\n\r\n";
            builder += list[index+1]+"\r\n";
            ibret = send(sock,builder.c_str(),builder._length(),0); 
            sendlength+=ibret;
            builder = "";
        }
    }

    hString footer = "--"+boundary+"--\r\n\r\n";
    ibret = send(sock,footer,footer._length(),0);
    sendlength+=ibret;

    show_message("before recv"," content-length:",length," sendlength:",sendlength);
    hString RequestResult = "";
    while(1){
        memset(tempbuf,0,sizeof(tempbuf));
        ibret = recv(sock,tempbuf,sizeof(tempbuf),0);
        if(ibret ==0 ) break;
        if(listener.OnReceiveData!=nullptr)
            listener.OnReceiveData(tempbuf);
        else
            RequestResult+=tempbuf;
    }

    if(listener.OnComplete !=nullptr){
        listener.OnComplete(RequestResult);
    }

    show_message("recv:",RequestResult);

    #ifdef WIN32
    closesocket(sock);
    WSACleanup();
    #else
    if(sock) shutdown(sock,2);
    #endif

    return true;
}

hString Post(const hString& url,const hString& data,long timeout,TransListener listener,std::function<void(Request& req)> OtherSetting ){
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

    auto urlkeyvalue = CutUrl(url);
    int port = urlkeyvalue._key.Contain(":")?hString::ToL(urlkeyvalue._key.Cut(":")._value):80;
    auto address = urlkeyvalue._key.Contain(":")?urlkeyvalue._key.Cut(":")._key:urlkeyvalue._key;
    address = address.Contain("http")?address.Cut("/",3)._value:address;

    sockaddr_in sockAddr;
    memset(&sockAddr,0,sizeof(sockAddr));
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_addr.s_addr = inet_addr(address.c_str());
    sockAddr.sin_port = htons(port);

    setsockopt(sock,SOL_SOCKET,SO_TIMESTAMP,&timeout,sizeof(timeout));

    auto ibret = connect(sock,reinterpret_cast<sockaddr*>(&sockAddr),sizeof(sockAddr));
    if(ibret!=0) return -1;  

    hString builder = "\r\n"+data;
    long length = builder._length();

    Request request(url);
    request.ContentLength = hString(length);
    request.ContentType = "application/x-www-form-urlencoded; charset=UTF-8";
    request.Connection = "close";
    if(OtherSetting!=nullptr) OtherSetting(request);
    auto header = request.ToString();
    ibret = send(sock,header.c_str(),header._length(),0);

    ibret = send(sock,builder.c_str(),header._length(),0);
    hString RequestResult = "";
    char tempbuf[4096];
    while(1){
        memset(tempbuf,0,sizeof(tempbuf));
        ibret = recv(sock,tempbuf,sizeof(tempbuf),0);
        if(ibret ==0 ) break;
        if(listener.OnReceiveData!=nullptr)
            listener.OnReceiveData(tempbuf);
        else
            RequestResult+=tempbuf;
    }
    if(listener.OnComplete !=nullptr){
        listener.OnComplete(RequestResult);
    }

    #ifdef WIN32
    closesocket(sock);
    WSACleanup();
    #else
    if(sock) shutdown(sock,2);
    #endif
    return RequestResult.substr(RequestResult.find("{"),RequestResult._length()-1);
}

}
