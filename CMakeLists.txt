cmake_minimum_required(VERSION 3.16)
project(HTTPServer LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(HTTPServer
    src/main.c
    src/network.c
    src/http.c
    src/server.c
    src/logger.c
    src/utility.c
)

target_include_directories(HTTPServer PRIVATE include)
target_include_directories(HTTPServer PRIVATE public)



if (WIN32)
  target_link_libraries(HTTPServer PRIVATE ws2_32)
endif()
