#include "server.h"
#include "http.h"
#include "utility.h"
#include "logger.h"
#include "config.h"

static const char *HttpStatusText400 = "400 Bad Request";
static const char *HttpStatusText404 = "404 Not Found";
static const char *HttpStatusText405 = "405 Method Not Allowed";

static const char *HttpBody400 = "<h1>400 Bad Request</h1>";
static const char *HttpBody404 = "<h1>404 Not Found</h1>";
static const char *HttpBody405 = "<h1>405 Method Not Allowed</h1>";

typedef struct {
    const char *urlRoute;
    const char *relativeFilePath;
} ServerRoute;

static const ServerRoute ServerRoutes[] = {
    { "/webpage.html", "webpage.html" },
    { "/file.txt",  "file.txt"  },
    { "/image.jpg",  "image.jpg"  },
    { 0, 0 }
};

SocketHandle ServerCreateListeningSocket(int portNumber)
{
    SocketHandle listeningSocket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address;
    int reuseOption = 1;

    if (listeningSocket == SOCKET_HANDLE_INVALID) {
        return SOCKET_HANDLE_INVALID;
    }

    (void)setsockopt(listeningSocket,
                     SOL_SOCKET,
                     SO_REUSEADDR,
                     (const char *)&reuseOption,
                     sizeof(reuseOption));

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons((unsigned short)portNumber);

    if (bind(listeningSocket, (struct sockaddr *)&address, sizeof(address)) == SOCKET_OPERATION_ERROR) {
        SocketClose(listeningSocket);
        return SOCKET_HANDLE_INVALID;
    }

    if (listen(listeningSocket, CONFIG_LISTEN_BACKLOG) == SOCKET_OPERATION_ERROR) {
        SocketClose(listeningSocket);
        return SOCKET_HANDLE_INVALID;
    }

    return listeningSocket;
}

static const ServerRoute* ServerFindRoute(const char *urlPath)
{
    for (int index = 0; ServerRoutes[index].urlRoute != 0; index++) {
        if (strcmp(urlPath, ServerRoutes[index].urlRoute) == 0) {
           
            return &ServerRoutes[index];
        }
    }
    return 0;
}

void ServerHandleClientConnection(SocketHandle clientSocket,
                                  const char *clientIpAddress,
                                  const char *rootDirectory)
{
    char requestLine[CONFIG_REQUEST_LINE_MAXIMUM];
    char httpMethod[CONFIG_METHOD_MAXIMUM];
    char urlPath[CONFIG_PATH_MAXIMUM];

    int httpStatusCode = 500;
    int isHeadRequest = 0;

    if (HttpReceiveRequestLine(clientSocket, requestLine, (int)sizeof(requestLine)) <= 0) {
        return;
    }

    if (HttpParseRequestLine(requestLine,
                             httpMethod, (int)sizeof(httpMethod),
                             urlPath, (int)sizeof(urlPath)) != 0) {
        httpStatusCode = 400;
        LoggerWriteHttpRequest(clientIpAddress, "-", "-", httpStatusCode);
        (void)HttpSendSimpleHtmlResponse(clientSocket, HttpStatusText400, HttpBody400);
        return;
    }

    if (HttpDiscardHeaders(clientSocket, CONFIG_MAXIMUM_HEADER_BYTES) != 0) {
        httpStatusCode = 400;
        LoggerWriteHttpRequest(clientIpAddress, httpMethod, urlPath, httpStatusCode);
        (void)HttpSendSimpleHtmlResponse(clientSocket, HttpStatusText400, HttpBody400);
        return;
    }

    if (UtilityValidateUrlPath(urlPath) != 0) {
        httpStatusCode = 400;
        LoggerWriteHttpRequest(clientIpAddress, httpMethod, urlPath, httpStatusCode);
        (void)HttpSendSimpleHtmlResponse(clientSocket, HttpStatusText400, HttpBody400);
        return;
    }

    HttpStripQueryString(urlPath);

    if (strcmp(urlPath, "/") == 0) {
        UtilityStringCopy(urlPath, "/webpage.html", (int)sizeof(urlPath));
    }

    if (strcmp(httpMethod, "GET") == 0) {
        isHeadRequest = 0;
    } else if (strcmp(httpMethod, "HEAD") == 0) {
        isHeadRequest = 1;
    } else {
        httpStatusCode = 405;
        LoggerWriteHttpRequest(clientIpAddress, httpMethod, urlPath, httpStatusCode);
        (void)HttpSendSimpleHtmlResponse(clientSocket, HttpStatusText405, HttpBody405);
        return;
    }

    const ServerRoute *matchedRoute = ServerFindRoute(urlPath);
    
    if (!matchedRoute) {
        httpStatusCode = 404;
        LoggerWriteHttpRequest(clientIpAddress, httpMethod, urlPath, httpStatusCode);
        (void)HttpSendSimpleHtmlResponse(clientSocket, HttpStatusText404, HttpBody404);
        return;
    }

    {
        char fullFilePath[CONFIG_PATH_MAXIMUM + 64];
        if (UtilityJoinPath(fullFilePath, (int)sizeof(fullFilePath), rootDirectory, matchedRoute->relativeFilePath) != 0) {
            httpStatusCode = 500;
            LoggerWriteHttpRequest(clientIpAddress, httpMethod, urlPath, httpStatusCode);
            (void)HttpSendSimpleHtmlResponse(clientSocket,
                                             "500 Internal Server Error",
                                             "<h1>500 Internal Server Error</h1>");
            return;
        }

        const char *mimeType = UtilityGetMimeTypeFromPath(matchedRoute->relativeFilePath);

        if (HttpSendFileResponse(clientSocket, fullFilePath, mimeType, isHeadRequest) == 0) {
            httpStatusCode = 200;
            LoggerWriteHttpRequest(clientIpAddress, httpMethod, urlPath, httpStatusCode);
        } else {
            httpStatusCode = 404;
            LoggerWriteHttpRequest(clientIpAddress, httpMethod, urlPath, httpStatusCode);
            (void)HttpSendSimpleHtmlResponse(clientSocket, HttpStatusText404, HttpBody404);
        }
    }
}
