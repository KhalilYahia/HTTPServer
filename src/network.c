#include "network.h"

#ifdef _WIN32
int NetworkInitialize(void)
{
    WSADATA winsockData;
    return WSAStartup(MAKEWORD(2, 2), &winsockData);
}

void NetworkCleanup(void)
{
    WSACleanup();
}
#else
int NetworkInitialize(void) { return 0; }
void NetworkCleanup(void) { }
#endif

int NetworkSendAll(SocketHandle socketHandle, const char *buffer, int bufferLength)
{
    int totalBytesSent = 0;

    while (totalBytesSent < bufferLength) {
        int bytesSent = (int)send(socketHandle,
                                 buffer + totalBytesSent,
                                 bufferLength - totalBytesSent,
                                 0);
        if (bytesSent <= 0) {
            return -1;
        }
        totalBytesSent += bytesSent;
    }

    return 0;
}
