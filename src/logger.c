#include "logger.h"
#include "platform.h"

#if CONFIG_LOGGING_STDOUT_ENABLE
    #include <stdio.h>
#endif

#if CONFIG_LOGGING_TIMESTAMP_ENABLE
    #include <time.h>
#endif

static void LoggerWriteInternal(const char *levelText, const char *message)
{
#if CONFIG_LOGGING_STDOUT_ENABLE
#if CONFIG_LOGGING_TIMESTAMP_ENABLE
    char timestampBuffer[32];
    time_t now = time(NULL);
    struct tm *timeInfo = localtime(&now);

    if (timeInfo) {
        /* YYYY-MM-DD HH:MM:SS */
        snprintf(timestampBuffer, sizeof(timestampBuffer),
                 "%04d-%02d-%02d %02d:%02d:%02d",
                 timeInfo->tm_year + 1900,
                 timeInfo->tm_mon + 1,
                 timeInfo->tm_mday,
                 timeInfo->tm_hour,
                 timeInfo->tm_min,
                 timeInfo->tm_sec);

        printf("[%s] %s %s\n",
               timestampBuffer,
               levelText ? levelText : "",
               message ? message : "-");
    } else {
        printf("%s %s\n",
               levelText ? levelText : "",
               message ? message : "-");
    }
#else
    printf("%s %s\n", levelText ? levelText : "", message ? message : "-");
#endif
    fflush(stdout);
#else
    (void)levelText;
    (void)message;
#endif
}

void LoggerWriteError(const char *message)
{
    if (CONFIG_LOGGING_LEVEL >= 1) {
        LoggerWriteInternal("[ERROR]", message);
    }
}

void LoggerWriteWarning(const char *message)
{
    if (CONFIG_LOGGING_LEVEL >= 2) {
        LoggerWriteInternal("[WARN ]", message);
    }
}

void LoggerWriteInformation(const char *message)
{
    if (CONFIG_LOGGING_LEVEL >= 3) {
        LoggerWriteInternal("[INFO ]", message);
    }
}



void LoggerWriteHttpRequest(const char *clientIpAddress,
                            const char *httpMethod,
                            const char *urlPath,
                            int httpStatusCode)
{
#if CONFIG_LOGGING_LEVEL >= 3
#if CONFIG_LOGGING_STDOUT_ENABLE
#if CONFIG_LOGGING_TIMESTAMP_ENABLE
    char timestampBuffer[32];
    time_t now = time(NULL);
    struct tm *timeInfo = localtime(&now);

    if (timeInfo) {
        snprintf(timestampBuffer, sizeof(timestampBuffer),
                 "%04d-%02d-%02d %02d:%02d:%02d",
                 timeInfo->tm_year + 1900,
                 timeInfo->tm_mon + 1,
                 timeInfo->tm_mday,
                 timeInfo->tm_hour,
                 timeInfo->tm_min,
                 timeInfo->tm_sec);

        printf("[%s] %s \"%s %s\" %d\n",
               timestampBuffer,
               clientIpAddress ? clientIpAddress : "-",
               httpMethod ? httpMethod : "-",
               urlPath ? urlPath : "-",
               httpStatusCode);
    } else {
        printf("%s \"%s %s\" %d\n",
               clientIpAddress ? clientIpAddress : "-",
               httpMethod ? httpMethod : "-",
               urlPath ? urlPath : "-",
               httpStatusCode);
    }
#else
    printf("%s \"%s %s\" %d\n",
           clientIpAddress ? clientIpAddress : "-",
           httpMethod ? httpMethod : "-",
           urlPath ? urlPath : "-",
           httpStatusCode);
#endif
    fflush(stdout);
#else
    (void)clientIpAddress; (void)httpMethod; (void)urlPath; (void)httpStatusCode;
#endif
#else
    (void)clientIpAddress; (void)httpMethod; (void)urlPath; (void)httpStatusCode;
#endif
}
