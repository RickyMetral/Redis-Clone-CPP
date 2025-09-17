
#include "TCPConnection.hpp"
#include <vector>

#define BACKLOG 10

class TCPServer : public TCPConnection {
protected:
    int32_t establishEndpoint(int32_t sockfd, struct addrinfo *p) override;//Wrapper to syscall bind()
    void reapDeadProcesses(struct sigaction& sa);//Makes sure to kill any forked processes
    void sigchld_handler(int32_t s);

public:
    TCPServer(const char* serverPort, int32_t sockFamily);//Sets sockfd to our server file descriptor. Unconnected when instantiated
    ~TCPServer() override;
    void queueConns();//Calls listen and queues any incoming connections, exits on fail
    virtual int32_t acceptConn(struct sockaddr* clientaddr);//Accepts one connection from the connection queue, returns -1 on fail, does not blocking I/O
    int32_t handleRequest(int32_t socketfd);//Receives clients msg and sends ACK. Blocks I/O
};