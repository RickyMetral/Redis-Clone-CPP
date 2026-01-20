cmake_minimum_required(VERSION 3.10)

# Project name
project(redis-clone)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output directory (optional)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Enable strict handling of warnings
# add_definitions("-Wall -Wextra -Werror")

# Source files
set(SERVER_SOURCES
    src/TCPClient.cpp
    src/TCPConnection.cpp
    src/TCPServer.cpp
    src/Epoll.cpp
    src/server.cpp
)

set(CLIENT_SOURCES
    src/TCPClient.cpp
    src/TCPConnection.cpp
    src/TCPServer.cpp
    src/client.cpp
)

set(NB_SERVER_SOURCES
    src/TCPClient.cpp
    src/TCPConnection.cpp
    src/TCPServer.cpp
    src/TCPServerNonBlock.cpp
    src/Epoll.cpp
    src/serverNonblock.cpp
)

# Executable
# add_executable(${PROJECT_NAME} ${SOURCES})
add_executable(redis-clone-server ${SERVER_SOURCES})
add_executable(redis-clone-client ${CLIENT_SOURCES})
add_executable(nonblock-redis-clone-server ${NB_SERVER_SOURCES})


# Include directories
# target_include_directories(${PROJECT_NAME} PRIVATE
#     ${CMAKE_SOURCE_DIR}/include
# )
target_include_directories(redis-clone-server PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)
target_include_directories(redis-clone-client PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)
target_include_directories(nonblock-redis-clone-server PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

