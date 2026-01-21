#ifndef LOGGER_H
#define LOGGER_H

#include "config.h"

/* Minimal logger interface. */

void LoggerWriteError(const char *message);
void LoggerWriteWarning(const char *message);
void LoggerWriteInformation(const char *message);

/* Request log */
void LoggerWriteHttpRequest(const char *clientIpAddress,
                            const char *httpMethod,
                            const char *urlPath,
                            int httpStatusCode);

#endif /* LOGGER_H */
