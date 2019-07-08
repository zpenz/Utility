#include <iostream>
#include <fstream>
#include "stranslate.hpp"

int main(int argc, char const *argv[]){
    #ifdef WIN32
    WSAData wsa;
    if (::WSAStartup(MAKEWORD(1,1),&wsa) != 0)
    {
        std::cout<<"WSAStartup error"<<std::endl;
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
    sockAddr.sin_addr.s_addr = inet_addr("192.168.10.250");                                     
          
    auto iRuse = "1";
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, iRuse, sizeof(iRuse));
    int nSendBuf=16*1024;
    setsockopt(sock,SOL_SOCKET,SO_SNDBUF,(const char*)&nSendBuf,sizeof(int)); 
    // int nNodelay = 1;
    // setsockopt(sock,SOL_SOCKET,TCP_NODELAY,(const char*)&nNodelay,sizeof(int));
   
    auto ibret = connect(sock,reinterpret_cast<sockaddr*>(&sockAddr),sizeof(sockAddr));
    if(ibret!=0) return 0;              

    FILE_PACKAGE tempPackage;
    std::fstream reader;
    reader.open(argv[1],std::ios::in|std::ios::binary);
   
    memset(tempPackage.package_content,0,sizeof(tempPackage.package_content));   
    memset(tempPackage.file_name,0,sizeof(tempPackage.file_name));
    reader.seekg(0,reader.end);
    size_t fileSize = reader.tellg();
    reader.seekg(0,reader.beg);

    auto index = 1;
    char buf[MAX_READ_SIZE];

    int iTotalSendSize = 0;
    while(1){
        memset(&tempPackage,0,sizeof(tempPackage));
        memset(tempPackage.package_content,0,sizeof(tempPackage.package_content));
        memset(tempPackage.file_name,0,sizeof(tempPackage.file_name));

        reader.read(tempPackage.package_content,MAX_READ_SIZE);
        auto read_size = reader.gcount();
        tempPackage.head.package_size = sizeof(tempPackage);
        tempPackage.head.datakind = 1;
        std::cout<<tempPackage.head.package_size<<std::endl;
        tempPackage.file_info_size = read_size;
        tempPackage.file_total_size = fileSize;
        tempPackage.index=index++;
        memcpy(tempPackage.file_name,"testfile!",strlen("testfile!"));

        ibret = send(sock,reinterpret_cast<char*>(&tempPackage),sizeof(tempPackage),0); 
        iTotalSendSize+=ibret;
        if(ibret<=0) {   
                #ifdef WIN32
                int iError_code = WSAGetLastError();                   
                std::cout<<"Error_code:"<<iError_code<<std::endl;  
                #endif
                exit(-1);                                             
        }  

        TRANSFORM_STATE state;
        memset(&state,0,sizeof(TRANSFORM_STATE));
        state.head.package_size = sizeof(TRANSFORM_STATE); 
        state.head.datakind = 0; 
        memcpy(state.message,"send",strlen("send"));
        ibret = send(sock,reinterpret_cast<char*>(&state),sizeof(state),0);
        if(ibret<=0) {
                #ifdef WIN32
                int iError_code = WSAGetLastError();
                std::cout<<"Error_code:"<<iError_code<<std::endl;
                #endif
                exit(-1);     
            }
        iTotalSendSize+=ibret;


        if(reader.eof()) {
            memset(&state,0,sizeof(TRANSFORM_STATE));
            state.head.package_size = sizeof(TRANSFORM_STATE); 
            state.head.datakind = 0;
            memcpy(state.message,"final",strlen("final"));
            ibret = send(sock,reinterpret_cast<char*>(&state),sizeof(state),0);
            if(ibret<=0) {
                #ifdef Win32
                int iError_code = WSAGetLastError();
                std::cout<<"Error_code:"<<iError_code<<std::endl;
                #endif
                exit(-1);
            }
            iTotalSendSize+=ibret;
            reader.close();

            std::cout<<"recv.."<<std::endl;
            // while(recv(sock,buf,sizeof(TRANSFORM_STATE),0)<0);
            // std::cout<<((TRANSFORM_STATE*)buf)->message<<std::endl;
            break;
        }

        // while(recv(sock,buf,sizeof(TRANSFORM_STATE),0)<0);
    }

    std::cout<<"Total Send: "<<iTotalSendSize<<std::endl;

    std::cout<<"Wait For End.."<<std::endl;
    // system("pause");
    getchar();
    #ifdef WIN32
    closesocket(sock);
    WSACleanup();
    #else
    if(sock) shutdown(sock,2);
    #endif
    return 0; 
}
