#include "utility.h"

void UtilityStringCopy(char *destination, const char *source, int destinationCapacity)
{
    int index = 0;

    if (!destination || destinationCapacity <= 0) {
        return;
    }
    if (!source) {
        destination[0] = '\0';
        return;
    }

    while (source[index] != '\0' && index < destinationCapacity - 1) {
        destination[index] = source[index];
        index++;
    }
    destination[index] = '\0';
}

int UtilityJoinPath(char *outputPath, int outputCapacity,
                    const char *rootDirectory, const char *relativeFilePath)
{
    int outputIndex = 0;
    int relativeIndex = 0;

    if (!outputPath || outputCapacity <= 0 || !rootDirectory || !relativeFilePath) {
        return -1;
    }

    while (rootDirectory[outputIndex] != '\0' && outputIndex < outputCapacity - 1) {
        outputPath[outputIndex] = rootDirectory[outputIndex];
        outputIndex++;
    }
    if (rootDirectory[outputIndex] != '\0') {
        return -1; /* overflow */
    }

    if (outputIndex > 0 &&
        outputPath[outputIndex - 1] != '/' &&
        outputPath[outputIndex - 1] != '\\') {
        if (outputIndex >= outputCapacity - 1) {
            return -1;
        }
        outputPath[outputIndex++] = '/';
    }

    while (relativeFilePath[relativeIndex] != '\0' && outputIndex < outputCapacity - 1) {
        outputPath[outputIndex++] = relativeFilePath[relativeIndex++];
    }
    if (relativeFilePath[relativeIndex] != '\0') {
        return -1; /* overflow */
    }

    outputPath[outputIndex] = '\0';
    return 0;
}

static int UtilityContainsDotDotSequence(const char *text)
{
    for (int index = 0; text[index] != '\0'; index++) {
        if (text[index] == '.' && text[index + 1] == '.') {
            return 1;
        }
    }
    return 0;
}

int UtilityValidateUrlPath(const char *urlPath)
{
    if (!urlPath) {
        return -1;
    }
    if (urlPath[0] != '/') {
        return -1;
    }

    for (int index = 0; urlPath[index] != '\0'; index++) {
        unsigned char character = (unsigned char)urlPath[index];

        if (character < 0x20) {
            return -1; /* control characters */
        }
        if (urlPath[index] == '\\') {
            return -1; /* reject backslashes */
        }
    }

    if (UtilityContainsDotDotSequence(urlPath)) {
        return -1;
    }

    return 0;
}

static int UtilityEndsWith(const char *text, const char *suffix)
{
    int textLength = 0;
    int suffixLength = 0;

    while (text[textLength] != '\0') {
        textLength++;
    }
    while (suffix[suffixLength] != '\0') {
        suffixLength++;
    }

    if (suffixLength > textLength) {
        return 0;
    }

    return (memcmp(text + (textLength - suffixLength), suffix, (size_t)suffixLength) == 0);
}

const char* UtilityGetMimeTypeFromPath(const char *filePath)
{
    if (!filePath) {
        return "application/octet-stream";
    }

    if (UtilityEndsWith(filePath, ".html") || UtilityEndsWith(filePath, ".htm")) {
        return "text/html; charset=utf-8";
    }
    if (UtilityEndsWith(filePath, ".css")) {
        return "text/css";
    }
    if (UtilityEndsWith(filePath, ".js")) {
        return "application/javascript";
    }
    if (UtilityEndsWith(filePath, ".png")) {
        return "image/png";
    }
    if (UtilityEndsWith(filePath, ".jpg") || UtilityEndsWith(filePath, ".jpeg")) {
        return "image/jpeg";
    }
    if (UtilityEndsWith(filePath, ".gif")) {
        return "image/gif";
    }
    if (UtilityEndsWith(filePath, ".txt")) {
        return "text/plain; charset=utf-8";
    }

    return "application/octet-stream";
}
