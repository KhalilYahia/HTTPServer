#ifndef NETWORK_H
#define NETWORK_H

#include "platform.h"

int NetworkInitialize(void);
void NetworkCleanup(void);

/* Sends all bytes in buffer, returns 0 on success, -1 on failure */
int NetworkSendAll(SocketHandle socketHandle, const char *buffer, int bufferLength);

#endif /* NETWORK_H */
