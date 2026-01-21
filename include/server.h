#ifndef SERVER_H
#define SERVER_H

#include "platform.h"

/* Create listening socket */
SocketHandle ServerCreateListeningSocket(int portNumber);

/* Handle one accepted client */
void ServerHandleClientConnection(SocketHandle clientSocket,
                                  const char *clientIpAddress,
                                  const char *rootDirectory);

#endif /* SERVER_H */
