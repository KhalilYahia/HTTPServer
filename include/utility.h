#ifndef UTILITY_H
#define UTILITY_H

#include "platform.h"

/* Safe string copy (always null-terminates if destinationCapacity>0) */
void UtilityStringCopy(char *destination, const char *source, int destinationCapacity);

/* Join rootDirectory + relativeFilePath (relativeFilePath must not start with '/').
   Returns 0 on success, -1 on overflow. */
int UtilityJoinPath(char *outputPath, int outputCapacity,
                    const char *rootDirectory, const char *relativeFilePath);

/* Basic URL path safety: returns 0 if safe, -1 if unsafe.
   Rejects "..", backslashes, control characters, and non-absolute paths. */
int UtilityValidateUrlPath(const char *urlPath);

/* Return MIME type based on file extension (small set). */
const char* UtilityGetMimeTypeFromPath(const char *filePath);

#endif /* UTILITY_H */
