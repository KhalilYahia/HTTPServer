#include "http.h"
#include "network.h"
#include "config.h"

static long HttpGetFileSizeBytes(const char *filePath)
{
    FileStatusStruct fileStatus;
    if (FileStatusFunction(filePath, &fileStatus) != 0) {
        return -1;
    }
    return (long)fileStatus.st_size;
}

static int HttpConvertUnsignedToDecimal(unsigned long value, char *outputDigits, int outputCapacity)
{
    int digitCount = 0;

    if (outputCapacity <= 0) {
        return 0;
    }

    if (value == 0) {
        outputDigits[0] = '0';
        return 1;
    }

    while (value > 0 && digitCount < outputCapacity) {
        outputDigits[digitCount++] = (char)('0' + (value % 10));
        value /= 10;
    }

    for (int leftIndex = 0, rightIndex = digitCount - 1; leftIndex < rightIndex; leftIndex++, rightIndex--) {
        char temp = outputDigits[leftIndex];
        outputDigits[leftIndex] = outputDigits[rightIndex];
        outputDigits[rightIndex] = temp;
    }

    return digitCount;
}

int HttpReceiveRequestLine(SocketHandle clientSocket, char *buffer, int bufferCapacity)
{
    int index = 0;
    char character = 0;

    if (bufferCapacity <= 1) {
        return -1;
    }

    while (index < bufferCapacity - 1) {
        int receivedBytes = (int)recv(clientSocket, &character, 1, 0);
        if (receivedBytes <= 0) {
            return -1;
        }

        if (character == '\r') {
            int peekedBytes = (int)recv(clientSocket, &character, 1, MSG_PEEK);
            if (peekedBytes > 0 && character == '\n') {
                (void)recv(clientSocket, &character, 1, 0);
            }
            break;
        }
        if (character == '\n') {
            break;
        }

        buffer[index++] = character;
    }

    buffer[index] = '\0';
    return index;
}

int HttpDiscardHeaders(SocketHandle clientSocket, int maximumBytesToDiscard)
{
    int totalBytesDiscarded = 0;
    int previousWasNewline = 0;

    while (totalBytesDiscarded < maximumBytesToDiscard) {
        char character = 0;
        int receivedBytes = (int)recv(clientSocket, &character, 1, 0);
        if (receivedBytes <= 0) {
            return -1;
        }
        totalBytesDiscarded++;

        if (character == '\n') {
            if (previousWasNewline) {
                return 0; /* found blank line */
            }
            previousWasNewline = 1;
        } else if (character == '\r') {
            /* ignore */
        } else {
            previousWasNewline = 0;
        }
    }

    return -1;
}

int HttpParseRequestLine(const char *requestLine,
                         char *httpMethod, int httpMethodCapacity,
                         char *urlPath, int urlPathCapacity)
{
    int index = 0;
    int methodIndex = 0;
    int pathIndex = 0;

    while (requestLine[index] == ' ') {
        index++;
    }

    while (requestLine[index] != '\0' && requestLine[index] != ' ') {
        if (methodIndex >= httpMethodCapacity - 1) {
            return -1;
        }
        httpMethod[methodIndex++] = requestLine[index++];
    }
    httpMethod[methodIndex] = '\0';

    while (requestLine[index] == ' ') {
        index++;
    }

    while (requestLine[index] != '\0' && requestLine[index] != ' ') {
        if (pathIndex >= urlPathCapacity - 1) {
            return -1;
        }
        urlPath[pathIndex++] = requestLine[index++];
    }
    urlPath[pathIndex] = '\0';

    if (methodIndex == 0 || pathIndex == 0) {
        return -1;
    }

    return 0;
}

void HttpStripQueryString(char *urlPath)
{
    for (int index = 0; urlPath[index] != '\0'; index++) {
        if (urlPath[index] == '?') {
            urlPath[index] = '\0';
            return;
        }
    }
}

int HttpSendSimpleHtmlResponse(SocketHandle clientSocket,
                               const char *httpStatusText,
                               const char *htmlBody)
{
    char decimalBuffer[32];
    int bodyLength = (htmlBody ? (int)strlen(htmlBody) : 0);
    int decimalLength = HttpConvertUnsignedToDecimal((unsigned long)bodyLength,
                                                     decimalBuffer,
                                                     (int)sizeof(decimalBuffer));

    if (NetworkSendAll(clientSocket, "HTTP/1.1 ", LITERAL_LENGTH("HTTP/1.1 ")) != 0) return -1;
    if (NetworkSendAll(clientSocket, httpStatusText, (int)strlen(httpStatusText)) != 0) return -1;

    if (NetworkSendAll(clientSocket,
        "\r\nContent-Type: text/html; charset=utf-8\r\nContent-Length: ",
        LITERAL_LENGTH("\r\nContent-Type: text/html; charset=utf-8\r\nContent-Length: ")) != 0) return -1;

    if (NetworkSendAll(clientSocket, decimalBuffer, decimalLength) != 0) return -1;

    if (NetworkSendAll(clientSocket, "\r\nConnection: close\r\n\r\n",
        LITERAL_LENGTH("\r\nConnection: close\r\n\r\n")) != 0) return -1;

    if (bodyLength > 0) {
        if (NetworkSendAll(clientSocket, htmlBody, bodyLength) != 0) return -1;
    }

    return 0;
}

int HttpSendFileResponse(SocketHandle clientSocket,
                         const char *filePath,
                         const char *contentType,
                         int isHeadRequest)
{
    char transferBuffer[CONFIG_FILE_CHUNK_SIZE];
    char decimalBuffer[32];
   
    long fileSize = HttpGetFileSizeBytes(filePath);
    if (fileSize < 0) {
        return -1;
    }

    int fileDescriptor = FileOpenFunction(filePath, FILE_OPEN_READ_ONLY_FLAG | FILE_OPEN_BINARY_FLAG);
   
    if (fileDescriptor < 0) {
        return -1;
    }

    int decimalLength = HttpConvertUnsignedToDecimal((unsigned long)fileSize,
                                                     decimalBuffer,
                                                     (int)sizeof(decimalBuffer));

    if (NetworkSendAll(clientSocket, "HTTP/1.1 200 OK\r\nContent-Type: ",
        LITERAL_LENGTH("HTTP/1.1 200 OK\r\nContent-Type: ")) != 0) { FileCloseFunction(fileDescriptor); return -1; }

    if (!contentType) contentType = "application/octet-stream";
    if (NetworkSendAll(clientSocket, contentType, (int)strlen(contentType)) != 0) { FileCloseFunction(fileDescriptor); return -1; }

    if (NetworkSendAll(clientSocket, "\r\nContent-Length: ",
        LITERAL_LENGTH("\r\nContent-Length: ")) != 0) { FileCloseFunction(fileDescriptor); return -1; }

    if (NetworkSendAll(clientSocket, decimalBuffer, decimalLength) != 0) { FileCloseFunction(fileDescriptor); return -1; }

    if (NetworkSendAll(clientSocket, "\r\nConnection: close\r\n\r\n",
        LITERAL_LENGTH("\r\nConnection: close\r\n\r\n")) != 0) { FileCloseFunction(fileDescriptor); return -1; }

    if (isHeadRequest) {
    
        FileCloseFunction(fileDescriptor);
        return 0;
    }

    for (;;) {
        int bytesRead = (int)FileReadFunction(fileDescriptor,
                                              transferBuffer,
                                              (unsigned int)sizeof(transferBuffer));
        if (bytesRead <= 0) {
            break;
        }
        if (NetworkSendAll(clientSocket, transferBuffer, bytesRead) != 0) {
            break;
        }
    }

    FileCloseFunction(fileDescriptor);
    
    return 0;
}
