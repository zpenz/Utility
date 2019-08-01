#ifdef MSVC
#pragma comment(lib,"ws2_32.lib")
#endif

#ifndef WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
typedef int SOCKET;
typedef sockaddr SOCKADDR;
// using DWORD  = unsigned int;
typedef unsigned int DWORD;
#else
#include <WinSock2.h>
#include <ws2tcpip.h>
#endif

#include "HttpUtility.hpp"
using namespace Utility;

#define MAX_BUFFER 5120
#define MAX_FILE_NAME 1024
#define MAX_SID_LENGTH 64
#define MAX_READ_SIZE 4096

struct PACKAGE_HEAD{
    int version;
    int datakind;
    int package_size;
};

struct FILE_PACKAGE{
        PACKAGE_HEAD head;
        int file_info_size;
        int file_total_size;
        int index;
        int uid;
        char file_name[MAX_FILE_NAME];
        char package_content[MAX_READ_SIZE];
        char sid[MAX_SID_LENGTH];
};

struct TRANSFORM_STATE{
    PACKAGE_HEAD head;
    int result_code;
    char message[MAX_FILE_NAME];
};

struct ReceviceListner{
    typedef function<void(hString& buf,Request& req,long index)> _recvfun;
    _recvfun OnHandleData;
    ReceviceListner(_recvfun datalistener=nullptr):OnHandleData(datalistener){}
};

extern bool start(ReceviceListner::_recvfun func);