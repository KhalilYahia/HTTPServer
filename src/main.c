#include "platform.h"
#include "network.h"
#include "server.h"
#include "logger.h"

#if CONFIG_LOGGING_STDOUT_ENABLE
    #include <stdio.h>
#endif



int main()
{
    int portNumber = 8080;
    const char *rootDirectory = "public";


    if (NetworkInitialize() != 0) {
        LoggerWriteError("Network initialization failed");
        return 1;
    }

    SocketHandle listeningSocket = ServerCreateListeningSocket(portNumber);
    if (listeningSocket == SOCKET_HANDLE_INVALID) {
        LoggerWriteError("Failed to create listening socket");
        NetworkCleanup();
        return 1;
    }

#if CONFIG_LOGGING_STDOUT_ENABLE
    {
        char messageBuffer[128];
        snprintf(messageBuffer, sizeof(messageBuffer),
                 "Listening on port %d, root directory: %s",
                 portNumber, rootDirectory);
        LoggerWriteInformation(messageBuffer);
    }
#endif

    while(1) {
        struct sockaddr_in clientAddress;
        socklen_t clientAddressLength = (socklen_t)sizeof(clientAddress);

        SocketHandle clientSocket = accept(listeningSocket,
                                           (struct sockaddr*)&clientAddress,
                                           &clientAddressLength);
        if (clientSocket == SOCKET_HANDLE_INVALID) {
            LoggerWriteWarning("accept() failed; stopping server loop");
            break;
        }

        char clientIpBuffer[INET_ADDRSTRLEN];
        const char *clientIpAddress = inet_ntop(AF_INET,
                                                &clientAddress.sin_addr,
                                                clientIpBuffer,
                                                (socklen_t)sizeof(clientIpBuffer));
        if (!clientIpAddress) {
            clientIpAddress = "unknown";
        }

        ServerHandleClientConnection(clientSocket, clientIpAddress, rootDirectory);
        SocketClose(clientSocket);
    }

    SocketClose(listeningSocket);
    NetworkCleanup();
    return 0;
}
