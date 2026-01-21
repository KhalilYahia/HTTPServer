#ifndef PLATFORM_H
#define PLATFORM_H

#include "config.h"
#include <string.h>
#include <sys/stat.h>
#include <stddef.h>

#ifdef _WIN32
    #define _WIN32_WINNT 0x0600
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <io.h>
    #include <fcntl.h>
#else
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <fcntl.h>
#endif

/* socket handle type */
#ifdef _WIN32
    typedef SOCKET SocketHandle;
    #define SOCKET_HANDLE_INVALID INVALID_SOCKET
    #define SOCKET_OPERATION_ERROR SOCKET_ERROR
    #define SocketClose closesocket

    #define FileStatusStruct struct _stat
    #define FileStatusFunction _stat
    #define FileOpenFunction _open
    #define FileReadFunction _read
    #define FileCloseFunction _close
    #define FILE_OPEN_READ_ONLY_FLAG _O_RDONLY
    #define FILE_OPEN_BINARY_FLAG _O_BINARY
#else 
    typedef int SocketHandle;
    #define SOCKET_HANDLE_INVALID (-1)
    #define SOCKET_OPERATION_ERROR (-1)
    #define SocketClose close

    #define FileStatusStruct struct stat
    #define FileStatusFunction stat
    #define FileOpenFunction open
    #define FileReadFunction read
    #define FileCloseFunction close
    #define FILE_OPEN_READ_ONLY_FLAG O_RDONLY
    #define FILE_OPEN_BINARY_FLAG 0
#endif

#define LITERAL_LENGTH(StringLiteral) ((int)(sizeof(StringLiteral) - 1))

#endif /* PLATFORM_H */
