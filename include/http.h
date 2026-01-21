#ifndef HTTP_H
#define HTTP_H

#include "platform.h"

/* Reads request line only. Returns length or -1. */
int HttpReceiveRequestLine(SocketHandle clientSocket,
                           char *buffer,
                           int bufferCapacity);

/* Discard headers up to blank line, with max byte limit.
   Returns 0 on success, -1 on failure/too large. */
int HttpDiscardHeaders(SocketHandle clientSocket,
                       int maximumBytesToDiscard);

/* Parses "METHOD PATH HTTP/1.1" => method + path */
int HttpParseRequestLine(const char *requestLine,
                         char *httpMethod, int httpMethodCapacity,
                         char *urlPath, int urlPathCapacity);

/* Removes "?query" from path in-place */
void HttpStripQueryString(char *urlPath);

/* Sends status page (HTML) */
int HttpSendSimpleHtmlResponse(SocketHandle clientSocket,
                               const char *httpStatusText,
                               const char *htmlBody);

/* Sends file with 200 OK and given content-type.
   If isHeadRequest != 0, sends headers only. */
int HttpSendFileResponse(SocketHandle clientSocket,
                         const char *filePath,
                         const char *contentType,
                         int isHeadRequest);

#endif /* HTTP_H */
