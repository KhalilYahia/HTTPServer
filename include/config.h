#ifndef CONFIG_H
#define CONFIG_H

/*
    Low-resource configuration
*/

/* Maximum size for request line: "GET /path HTTP/1.1" */
#ifndef CONFIG_REQUEST_LINE_MAXIMUM
#define CONFIG_REQUEST_LINE_MAXIMUM 256
#endif

/* Maximum HTTP method length including null terminator */
#ifndef CONFIG_METHOD_MAXIMUM
#define CONFIG_METHOD_MAXIMUM 8
#endif

/* Maximum URL path length including null terminator */
#ifndef CONFIG_PATH_MAXIMUM
#define CONFIG_PATH_MAXIMUM 192
#endif

/* File streaming chunk size */
#ifndef CONFIG_FILE_CHUNK_SIZE
#define CONFIG_FILE_CHUNK_SIZE 512
#endif

/* Maximum header bytes to discard */
#ifndef CONFIG_MAXIMUM_HEADER_BYTES
#define CONFIG_MAXIMUM_HEADER_BYTES 2048
#endif

/* listen() backlog */
#ifndef CONFIG_LISTEN_BACKLOG
#define CONFIG_LISTEN_BACKLOG 8
#endif

/* Logging: 0=OFF, 1=ERROR, 2=WARNING, 3=INFO */
#ifndef CONFIG_LOGGING_LEVEL
#define CONFIG_LOGGING_LEVEL 3
#endif

/* If 1: include timestamps in logs (may be heavy for MCU) */
#ifndef CONFIG_LOGGING_TIMESTAMP_ENABLE
#define CONFIG_LOGGING_TIMESTAMP_ENABLE 1
#endif

/* If 1: print logs to stdout (PC)*/
#ifndef CONFIG_LOGGING_STDOUT_ENABLE
#define CONFIG_LOGGING_STDOUT_ENABLE 1
#endif

#if !CONFIG_LOGGING_STDOUT_ENABLE
  /* Prevent accidental stdio usage in “no-stdio” builds */
  #define printf(...)  DO_USE_PRINTF_IF_STDIO_ISNOT_DEFINED
  #define fprintf(...) DO_USE_FPRINTF_IF_STDIO_ISNOT_DEFINED
#endif

#endif /* CONFIG_H */
