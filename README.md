# Redis Clone in C++ ( STILL UNDER DEVELOPMENT)


A lightweight Redis server clone being implemented in C++ using epoll for high-performance I/O multiplexing and system socket libraries. This project demonstrates key-value store architecture with efficient concurrent client handling. 


### Core Components

- **TCPConnection**: Manages server socket creation and basic reading/writing functionality
- **TCPServer**: Handles listening and accepting connections and client request handling
- **TCPClient**: Allows for server querying and receive response
- **Epoll**: Wrapper for Linux epoll system calls. Handling of EPOLL is done in a user defined callback
- **Event-Driven Callback System**: Handles different types of events (new connections, client data, disconnections)


## 📋 Requirements

- **Operating System**: Linux (epoll is Linux-specific)
- **Compiler**: GCC with C++11 support or newer
- **Libraries**: 
  - Standard C++ libraries
  - POSIX socket libraries
  - Linux epoll system calls

## 🔧 Building the Project

```bash
# Clone the repository
git clone https://github.com/RickyMetral/REDIS-CLONE-C-.git
cd REDIS-CLONE-C-

# Compile the server
mkdir build && cd build
cmake ..
make

# Run the server
./bin/redis_server
```

## 🚦 Usage

### Starting the Server

```bash
./bin/redis_server
```

The server will start listening on the default port (typically 3490) and accept client connections.

## 📚 Learning Resources

This project demonstrates several important concepts:

- **Network Programming**: Socket creation, binding, and connection management
- **Event-Driven Programming**: Using epoll for efficient I/O multiplexing
- **Protocol Implementation**: Redis protocol parsing and response generation
- **System Programming**: Low-level system calls and resource management
